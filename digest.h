#ifndef _DIGEST_H_
#define _DIGEST_H_

#include <httpcommon.h>
class Digest {
   public:
    template <typename T>
    static std::string base64_encode(const T &data);

    template <typename T>
    static std::string base16_encode(const T &data);

    template <typename T>
    static T md5Hash(const T &input);

    template <typename T>
    static T sha256Hash(const T &input);

    template <typename T1, typename T2>
    static ByteArray hmac_sha256(const T1 &key, const T2 &content);

    template <typename T>
    static std::string md5_base64(const T &content);

    template <typename T>
    static std::string sha256_base64(const T &content);

    template <typename T>
    static std::string md5_base16(const T &content);

    template <typename T>
    static std::string sha256_base16(const T &content);
};

#include <digest.hpp>

#endif
