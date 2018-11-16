#ifndef _SERVER_SIDE_ENCRYPTION_H_
#define _SERVER_SIDE_ENCRYPTION_H_

#include <map>
#include <vector>
#include <string>

typedef std::vector<char> ByteArray;


ByteArray sumHmac(const void *key, size_t klen, const void *data, size_t dlen);
ByteArray sumHmac(const std::string &key, const std::string &content);
ByteArray sumHmac(const std::string &key, const ByteArray &content);
ByteArray sumHmac(const ByteArray &key, const std::string &content);

class Digest {
   public:

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

    template <typename T>
    static std::string md5_base64(const T &content) {
        return "";
    }

    template <typename T>
    static std::string sha256_base64(const T &content) {
        return "";
    }
};

class ServerSideEncryption {
   public:
    void marshall(std::map<std::string, std::string> &headers) {}
};

#endif
