#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <httpurl.h>
#include <set>
#include <list>

//------------------------------------------------------------------------------
typedef std::list<std::pair<std::string,std::string> > HeadersType;
class HeadersBuilder;
class Headers {
   public:
    Headers() {}
    Headers(const HeadersBuilder &);
    std::string toString();
    operator const std::string() { return toString(); }
    std::string get(const std::string &key);
    std::set<std::string> names();
    HeadersBuilder newBuilder() const;

   private:
    HeadersType headers_;
    friend class HeadersBuilder;
};

//------------------------------------------------------------------------------
class HeadersBuilder {
public:
    HeadersBuilder() {}
    HeadersBuilder(const Headers&);
    Headers build() const;
    HeadersBuilder &set(const std::string &key, const std::string &value);
private:
    HeadersType headers_;
    friend class Headers;
};

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

    const HttpUrl &url() { return url_; }
    std::string method() { return method_; }
    std::string header(const std::string &) { return ""; }
    Headers headers() { return headers_; }

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
