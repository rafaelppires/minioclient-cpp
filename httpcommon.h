#ifndef _HTTP_COMMON_H_
#define _HTTP_COMMON_H_

#include <map>
#include <string>
#include <vector>

namespace HttpStrings {
extern const std::string crlf;
extern const std::string http11;
extern const std::string user_agent;
extern const std::string content_len;
extern const std::string connection;
extern const std::string keepalive;
extern const std::string contenttype;
extern const std::string transfer_enc;
}

class HeadersBuilder;
class ReqRepBuilder {
   public:
    virtual std::string getHeaderValue(const std::string &) const = 0;
    virtual ReqRepBuilder &appendBody(const std::string &) = 0;
    virtual ReqRepBuilder &headers(const HeadersBuilder &) = 0;
};

typedef std::vector<char> ByteArray;
typedef std::map<std::string, std::string> KeyValueMap;

#endif
