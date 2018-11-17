#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <string>
#include <map>
#include <set>
#include <httprequest.h>

//------------------------------------------------------------------------------
class Response {
   public:
    bool empty() { return true; }
    std::string protocol() { return protocol_; }
    std::string body() { return body_; }
    int code() { return status_code_; }
    bool isSuccessful();
    Headers headers() const;

   private:
    std::string body_, protocol_;
    int status_code_;
};

//------------------------------------------------------------------------------
class Call {
   public:
    Response execute() { return Response(); }

   private:
};

//------------------------------------------------------------------------------
class HttpClient {
   public:
    Call newCall(const Request &);

   private:
};

#endif
