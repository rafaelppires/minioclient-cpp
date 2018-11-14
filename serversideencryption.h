#ifndef _SERVER_SIDE_ENCRYPTION_H_
#define _SERVER_SIDE_ENCRYPTION_H_

#include <map>
#include <string>

typedef std::vector<char> ByteArray;


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


class Digest {
   public:
    typedef std::vector<char> ByteArray;

    template <typename T>
    static T md5Hash(const T &input) {
        T ret;
        return ret;
    }
    template <typename T>
    static T sha256Hash(const T &input) {
        T ret;
        return ret;
    }
};

class ServerSideEncryption {
   public:
    void marshall(std::map<std::string, std::string> &headers) {}
};

#endif
