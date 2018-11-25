#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <httpcommon.h>
#include <httpheaders.h>
#include <httpurl.h>

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
class Request;
class RequestBody {
   public:
    // Constructors
    RequestBody() {}
    RequestBody(const std::string &contentType, ByteArray &&body);

    // Copy and move constructors
    RequestBody(RequestBody &&);
    RequestBody(const RequestBody &);

    // Copy and move assignments
    RequestBody &operator=(RequestBody &&);
    RequestBody &operator=(const RequestBody &);

    // Member functions
    void append(const std::string &content);

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
    std::string stringBody() const;
    std::string method() const;
    std::string header(const std::string &) const;
    std::string statusLine() const;
    std::string headerString() const;
    std::string httpHeader() const;

   private:
    HttpUrl url_;
    std::string method_, protocol_;
    Headers headers_;
    RequestBody body_;
    // Map<Class<?>, Object> tags;`
    friend class RequestBuilder;
};

//------------------------------------------------------------------------------
class RequestBuilder : public ReqRepBuilder{
   public:
    RequestBuilder();
    RequestBuilder(const Request &);
    RequestBuilder &url(const HttpUrl &);
    RequestBuilder &method(const std::string &method);
    RequestBuilder &protocol(const std::string &method);
    RequestBuilder &body(RequestBody &&body);
    RequestBuilder &header(const std::string &key, const std::string &value);
    RequestBuilder &headers(const HeadersBuilder &);
    RequestBuilder &appendBody(const std::string &);
    std::string getHeaderValue(const std::string &key) const;
    Request build();

   private:
    HttpUrl url_;
    std::string method_, protocol_;
    HeadersBuilder headers_;
    RequestBody body_;
    friend class Request;
};

//------------------------------------------------------------------------------

#endif
