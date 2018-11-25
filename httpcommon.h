#ifndef _HTTP_COMMON_H_
#define _HTTP_COMMON_H_

#include <map>
#include <string>
#include <vector>

class HeadersBuilder;
class HttpStrings {
public:
    static const std::string http11;
    static const std::string user_agent;
    static const std::string content_len;
    static const std::string connection;
    static const std::string keepalive;
    static const std::string contenttype;
    static const std::string transfer_enc;
};

class ReqRepBuilder {
public:
    virtual std::string getHeaderValue(const std::string &) const = 0;
    virtual ReqRepBuilder &appendBody(const std::string &) = 0;
    virtual ReqRepBuilder &headers(const HeadersBuilder &) = 0;
};

typedef std::vector<char> ByteArray;
typedef std::map<std::string, std::string> KeyValueMap;

#endif

