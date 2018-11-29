#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <assert.h>
//------------------------------------------------------------------------------
template< typename T1, typename T2 >
ByteArray Digest::hmac_sha256(const T1 &k, const T2 &content) {
    const void *key = k.data();
    size_t klen = k.size();
    auto data = (const unsigned char *)content.data();
    size_t dlen = content.size();
    unsigned char hmac[SHA256_DIGEST_LENGTH];
    unsigned int len;
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key, klen, EVP_sha256(), NULL);
    HMAC_Update(&ctx, data, dlen);
    HMAC_Final(&ctx, hmac, &len);
    HMAC_CTX_cleanup(&ctx);
    assert(len == SHA256_DIGEST_LENGTH);
    return ByteArray(hmac,hmac+sizeof(hmac));
}
//------------------------------------------------------------------------------
template <typename T>
std::string Digest::base64_encode(const T &data) {
    std::string ret;
    if (data.empty()) return ret;

    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);  // everything in one line
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    ret = std::string(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return ret;
}

//------------------------------------------------------------------------------
template <typename T>
std::string Digest::base16_encode(const T &data) {
    static const char *dict = "0123456789abcdef";
    std::string ret;
    for (const auto &i : data) {
        ret += dict[(((char)i) & 0xf0) >> 4];
        ret += dict[((char)i) & 0x0f];
    }
    return ret;
}

//------------------------------------------------------------------------------
template <typename T>
T Digest::md5Hash(const T &input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, input.data(), input.size());
    MD5_Final(hash, &md5);
    return T(hash, hash + sizeof(hash));
}

//------------------------------------------------------------------------------
template <typename T>
T Digest::sha256Hash(const T &input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.data(), input.size());
    SHA256_Final(hash, &sha256);
    return T(hash, hash + sizeof(hash));
}

//------------------------------------------------------------------------------
template <typename T>
std::string Digest::md5_base64(const T &content) {
    T ret = md5Hash(content);
    return base64_encode(ret);
}

//------------------------------------------------------------------------------
template <typename T>
std::string Digest::sha256_base64(const T &content) {
    T ret = sha256Hash(content);
    return base64_encode(ret);
}

//------------------------------------------------------------------------------
template <typename T>
std::string Digest::md5_base16(const T &content) {
    T ret = md5Hash(content);
    return base16_encode(ret);
}

//------------------------------------------------------------------------------
template <typename T>
std::string Digest::sha256_base16(const T &content) {
    T ret = sha256Hash(content);
    return base16_encode(ret);
}

//------------------------------------------------------------------------------
