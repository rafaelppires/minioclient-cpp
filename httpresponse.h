#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <httpheaders.h>
#include <stringutils.h>
#include <string>
#include <vector>
using namespace StringUtils;

//------------------------------------------------------------------------------
class ResponseBuilder;
class Response {
   public:
    // Constructors
    Response();
    Response(const ResponseBuilder &);

    // Move constructor and assignment
    Response(Response &&r);
    Response &operator=(Response &&r);

    // Member functions
    bool empty();
    bool isSuccessful();
    const Headers &headers() const;
    std::string protocol() { return protocol_; }
    const std::string &body() const { return body_; }
    int code() const { return code_; }
    std::string toString() const;

   private:
    int code_;
    std::string body_, protocol_, message_;
    Headers headers_;
    friend class ResponseBuilder;
};

//------------------------------------------------------------------------------
class ResponseBuilder {
   public:
    // Constructors
    ResponseBuilder() {}
    ResponseBuilder(const Response &);

    // Copy and move constructors
    ResponseBuilder(ResponseBuilder &&r);
    ResponseBuilder(const ResponseBuilder &r);

    // Copy and move assignments
    ResponseBuilder &operator=(const ResponseBuilder &r);
    ResponseBuilder &operator=(ResponseBuilder &&r);

    ResponseBuilder &protocol(const std::string &);
    ResponseBuilder &code(int);
    ResponseBuilder &message(const std::string &);
    ResponseBuilder &headers(const HeadersBuilder &);
    ResponseBuilder &appendBody(const std::string &chunk);
    std::string getHeaderValue(const std::string &k);
    Response build();

   private:
    int code_;
    std::string body_, protocol_, message_;
    HeadersBuilder headers_;
    friend class Response;
};

//------------------------------------------------------------------------------
class ResponseHeader {
   public:
    ResponseHeader();
    ResponseHeader(const Headers &);
    std::string etag() { return etag_; }

   private:
    std::string etag_;  // ETag
};

//------------------------------------------------------------------------------
class HttpResponse {
   public:
    HttpResponse() {}
    HttpResponse(HttpResponse &&r);
    HttpResponse &operator=(HttpResponse &&r);
    HttpResponse(const Response &r) : header_(r.headers()) {}
    ResponseHeader header() { return header_; }

   private:
    ResponseHeader header_;
    Response response_;
};

#endif
