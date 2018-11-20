#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <httpurl.h>
#include <httpheaders.h>
#include <httpcommon.h>

//------------------------------------------------------------------------------
class Request;
class RequestBody {
   public:
    RequestBody() {}
    RequestBody(const RequestBody&);
    RequestBody& operator=(const RequestBody&);
    RequestBody(RequestBody&&);
    RequestBody& operator=(RequestBody&&);
    RequestBody(const std::string &contentType, ByteArray &&body);
   private:
    ByteArray data_;
    std::string content_type_;
    friend class Request;
};

//------------------------------------------------------------------------------
class RequestBuilder;
class Request {
   public:
    Request() {}
    Request(const RequestBuilder &);

    const HttpUrl &url() const;
    const Headers &headers() const;
    bool hasBody() const;
    const ByteArray &body() const;
    std::string method() const;
    std::string header(const std::string &) const;
    std::string statusLine() const;
    std::string headerString() const;
    std::string httpHeader() const;

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
    void method(const std::string &method, RequestBody &&body);
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
