#include <serversideencryption.h>

ByteArray sumHmac(const void *key, size_t klen, const void *data, size_t dlen) {
    // unsigned char *ret = HMAC(EVP_sha256(), key, keylen, data, datalen,
    // result, resultlen);
    return ByteArray();
}

ByteArray sumHmac(const std::string &key, const std::string &content) {
    return sumHmac(key.data(),key.size(),content.data(),content.size());
}

ByteArray sumHmac(const std::string &key, const ByteArray &content) {
    return sumHmac(key.data(),key.size(),content.data(),content.size());
}

ByteArray sumHmac(const ByteArray &key, const std::string &content) {
    return sumHmac(key.data(),key.size(),content.data(),content.size());
}


#if 0
package io.minio;

import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Optional;

import javax.crypto.SecretKey;
import javax.security.auth.DestroyFailedException;
import javax.security.auth.Destroyable;

import  com.google.common.io.BaseEncoding;

import io.minio.errors.InvalidArgumentException;


/**
* ServerSideEncryption represents a form of server-side encryption.
*/
public abstract class ServerSideEncryption implements Destroyable {
 
  protected boolean destroyed = false;
 
   /**
    * Returns the type of server-side encryption.
    * @return the type of server-side encryption.
    */
  public abstract Type getType();
 
   /**
    * Set the server-side-encryption headers of this specific encryption.
    * @param headers The metadata key-value map.
    */
  public abstract void marshal(Map<String, String> headers);
 
 
  @Override
  public boolean isDestroyed() {
    return this.destroyed;
  }
 
   /**
    * The types of server-side encryption.
    */
  public static enum Type { 
    SSE_C, SSE_S3, SSE_KMS;

   /**
    * Returns true if the server-side encryption requires a TLS connection.
    * @return true if the type of server-side encryption requires TLS.
    */
    public boolean requiresTls() {
      return this.equals(SSE_C);
    }

   /**
    * Returns true if the server-side encryption requires signature V4.
    * @return true if the type of server-side encryption requires signature V4.
    */
    public boolean requiresV4() {
      return this.equals(SSE_KMS);
    }
  }

  private static boolean isCustomerKeyValid(SecretKey key) {
    if (key == null) {
      return false;
    }
    return !key.isDestroyed() && key.getAlgorithm().equals("AES") && key.getEncoded().length == 32;
  }

 /**
 * Create a new server-side-encryption object for encryption with customer
 * provided keys (a.k.a. SSE-C).
 *
 * @param key The secret AES-256 key.
 * @return An instance of ServerSideEncryption implementing SSE-C.
 * @throws InvalidKeyException if the provided secret key is not a 256 bit AES key.
 * @throws NoSuchAlgorithmException if the crypto provider does not implement MD5.
 */
  public static ServerSideEncryption withCustomerKey(SecretKey key) 
    throws InvalidKeyException, NoSuchAlgorithmException {
    if (!isCustomerKeyValid(key)) {
      throw new InvalidKeyException("The secret key is not a 256 bit AES key");
    }
    return new ServerSideEncryptionWithCustomerKey(key, MessageDigest.getInstance(("MD5")));
  }

 /**
 * Create a new server-side-encryption object for encryption with customer
 * provided keys (a.k.a. SSE-C).
 *
 * @param key The secret AES-256 key.
 * @return An instance of ServerSideEncryption implementing SSE-C.
 * @throws InvalidKeyException if the provided secret key is not a 256 bit AES key.
 * @throws NoSuchAlgorithmException if the crypto provider does not implement MD5.
 */
  public static ServerSideEncryption copyWithCustomerKey(SecretKey key) 
    throws InvalidKeyException, NoSuchAlgorithmException {
    if (!isCustomerKeyValid(key)) {
      throw new InvalidKeyException("The secret key is not a 256 bit AES key");
    }
    return new ServerSideEncryptionCopyWithCustomerKey(key, MessageDigest.getInstance(("MD5")));
  }

 /**
  * Create a new server-side-encryption object for encryption at rest (a.k.a. SSE-S3).
  *
  * @return an instance of ServerSideEncryption implementing SSE-S3  
  */
  public static ServerSideEncryption atRest() {
    return new ServerSideEncryptionS3(); 
  }

 /**
  * Create a new server-side-encryption object for encryption using a KMS (a.k.a. SSE-KMS).
  * 
  * @param keyId   specifies the customer-master-key (CMK) and must not be null.
  * @param context is the encryption context. If the context is null no context is used.
  *
  * @return an instance of ServerSideEncryption implementing SSE-KMS.  
  */
  public static ServerSideEncryption withManagedKeys(String keyId, Map<String,String> context) 
    throws InvalidArgumentException, UnsupportedEncodingException {
    if (keyId == null) {
      throw new InvalidArgumentException("The key-ID cannot be null");
    }
    if (context == null) {
      return new ServerSideEncryptionKms(keyId, Optional.empty());
    }

    StringBuilder builder = new StringBuilder();
    int i = 0;
    builder.append('{');
    for (Entry<String,String> entry : context.entrySet()) {
      builder.append('"');
      builder.append(entry.getKey());
      builder.append('"');
      builder.append(':');
      builder.append('"');
      builder.append(entry.getValue());
      builder.append('"');
      if (i < context.entrySet().size() - 1) {
        builder.append(',');
      }
    }
    builder.append('}');
    String contextString = BaseEncoding.base64().encode(builder.toString().getBytes("UTF-8"));
    return new ServerSideEncryptionKms(keyId, Optional.of(contextString)); 
  }

  static class ServerSideEncryptionWithCustomerKey extends ServerSideEncryption {

    protected final SecretKey secretKey;
    protected final MessageDigest md5;     
   
    public ServerSideEncryptionWithCustomerKey(SecretKey key, MessageDigest md5) {
      this.secretKey = key;
      this.md5 = md5;
    }

    @Override
    public final Type getType() { 
      return Type.SSE_C; 
    }
   
    @Override
    public void marshal(Map<String, String> headers) {
      if (this.isDestroyed()) {
        throw new IllegalStateException("object is already destroyed");
      }
      try {
        final byte[] key = secretKey.getEncoded();
        md5.update(key);
           
        headers.put("X-Amz-Server-Side-Encryption-Customer-Algorithm", "AES256");
        headers.put("X-Amz-Server-Side-Encryption-Customer-Key",  BaseEncoding.base64().encode(key));
        headers.put("X-Amz-Server-Side-Encryption-Customer-Key-Md5", BaseEncoding.base64().encode(md5.digest()));
      } finally {
        md5.reset();
      }    
    }
   
    @Override
    public final void destroy() throws DestroyFailedException {
      secretKey.destroy(); 
      this.destroyed = true;
    }
  }

  static final class ServerSideEncryptionCopyWithCustomerKey extends ServerSideEncryptionWithCustomerKey {
   
    public ServerSideEncryptionCopyWithCustomerKey(SecretKey key, MessageDigest md5) {
      super(key, md5);
    }

    @Override
    public final void marshal(Map<String, String> headers) {
      if (this.isDestroyed()) {
        throw new IllegalStateException("object is already destroyed");
      }
      try {
        final byte[] key = secretKey.getEncoded();
        md5.update(key);
        final String md5Sum = BaseEncoding.base64().encode(md5.digest());
        headers.put("X-Amz-Copy-Source-Server-Side-Encryption-Customer-Algorithm", "AES256");
        headers.put("X-Amz-Copy-Source-Server-Side-Encryption-Customer-Key",  BaseEncoding.base64().encode(key));
        headers.put("X-Amz-Copy-Source-Server-Side-Encryption-Customer-Key-Md5", md5Sum);
      } finally {
        md5.reset();
      }    
    }
  }

  static final class ServerSideEncryptionS3 extends ServerSideEncryption {
    @Override
    public final Type getType() { 
      return Type.SSE_S3; 
    }

    @Override
    public final void marshal(Map<String, String> headers) { 
      headers.put("X-Amz-Server-Side-Encryption", "AES256"); 
    }

    @Override
    public final void destroy() throws DestroyFailedException {
      this.destroyed = true;
    }
  }

  static final class ServerSideEncryptionKms extends ServerSideEncryption {

    final String keyId;
    final Optional<String> context;

    public ServerSideEncryptionKms(String keyId, Optional<String> context) {
      this.keyId = keyId;
      this.context = context;
    }

    @Override
    public final Type getType() { 
      return Type.SSE_KMS; 
    }

    @Override
    public final void marshal(Map<String, String> headers) {
      if (this.isDestroyed()) {
        throw new IllegalStateException("object is already destroyed");
      }
      headers.put("X-Amz-Server-Side-Encryption-Aws-Kms-Key-Id", keyId);
      headers.put("X-Amz-Server-Side-Encryption", "aws:kms");
      if (context.isPresent()) {
        headers.put("X-Amz-Server-Side-Encryption-Context", context.get()); 
      }
    }

    @Override
    public final void destroy() throws DestroyFailedException {
      this.destroyed = true;
    }
  }
}
#endif

