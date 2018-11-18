#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <httpurl.h>
#include <httpheaders.h>

//------------------------------------------------------------------------------
class RequestBody {
   public:
    static RequestBody create(const std::string &contentType,
                              const std::vector<char> &body) {
        return RequestBody();
    }
};

//------------------------------------------------------------------------------
class RequestBuilder;
class Request {
   public:
    Request() {}
    Request(const RequestBuilder &);

    const HttpUrl &url();
    const Headers &headers();
    std::string method();
    std::string header(const std::string &);
    std::string statusLine();
    std::string headerString();
    std::string httpHeader();

   private:
    HttpUrl url_;
    std::string method_;
    Headers headers_;
    RequestBody body_;
    // Map<Class<?>, Object> tags;`
    friend class RequestBuilder;
};

//------------------------------------------------------------------------------
class RequestBuilder {
   public:
    RequestBuilder();
    RequestBuilder(const Request &);
    void url(const HttpUrl &);
    void method(const std::string &method, const RequestBody &body);
    RequestBuilder &header(const std::string &key, const std::string &value);
    Request build();

   private:
    HttpUrl url_;
    std::string method_;
    HeadersBuilder headers_;
    RequestBody body_;
    friend class Request;
};

//------------------------------------------------------------------------------

#endif
