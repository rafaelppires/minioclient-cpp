#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <httpheaders.h>
#include <stringutils.h>
#include <string>
#include <vector>
using namespace StringUtils;

//------------------------------------------------------------------------------
class Method {
   public:
    enum Methods { HEAD, PUT, POST };
    Method(Methods m) : m_(m) {}
    operator Methods() { return m_; }
    std::string toString() {
        switch (m_) {
            case HEAD:
                return "HEAD";
            case PUT:
                return "PUT";
            case POST:
                return "POST";
        };
    }

   private:
    Methods m_;
};

//------------------------------------------------------------------------------
class ResponseBuilder;
class Response {
   public:
    Response();
    Response(Response &&r);
    Response &operator=(Response &&r);
    Response(const ResponseBuilder &);
    bool empty();
    bool isSuccessful();
    const Headers &headers() const;
    std::string protocol() { return protocol_; }
    const std::string &body() const { return body_; }
    int code() const { return code_; }

   private:
    int code_;
    std::string body_, protocol_, message_;
    Headers headers_;
    friend class ResponseBuilder;
};

//------------------------------------------------------------------------------
class ResponseBuilder {
   public:
    ResponseBuilder() {}
    ResponseBuilder(const ResponseBuilder &r);
    ResponseBuilder(ResponseBuilder &&r);
    ResponseBuilder &operator=(const ResponseBuilder &r);
    ResponseBuilder &operator=(ResponseBuilder &&r);
    ResponseBuilder(const Response &);
    ResponseBuilder &protocol(const std::string &);
    ResponseBuilder &code(int);
    ResponseBuilder &message(const std::string &);
    ResponseBuilder &headers(const HeadersBuilder &);
    ResponseBuilder &addBody(const std::string &chunk) {
        body_ += chunk;
        return *this;
    }
    std::string get(const std::string &k) { return headers_.get(k); }
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
