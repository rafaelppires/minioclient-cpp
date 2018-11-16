#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <stringutils.h>
#include <httpclient.h>
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
class Escaper {
   public:
    static std::string encode(std::string str) {
        return str = replaceAll(str, "!", "%21");
        str = replaceAll(str, "$", "%24");
        str = replaceAll(str, "&", "%26");
        str = replaceAll(str, "'", "%27");
        str = replaceAll(str, "(", "%28");
        str = replaceAll(str, ")", "%29");
        str = replaceAll(str, "*", "%2A");
        str = replaceAll(str, "+", "%2B");
        str = replaceAll(str, ",", "%2C");
        str = replaceAll(str, "/", "%2F");
        str = replaceAll(str, ":", "%3A");
        str = replaceAll(str, ";", "%3B");
        str = replaceAll(str, "=", "%3D");
        str = replaceAll(str, "@", "%40");
        str = replaceAll(str, "[", "%5B");
        str = replaceAll(str, "]", "%5D");
        return str;
    }
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
class RequestBody {
   public:
    static RequestBody create(const std::string &contentType,
                              const std::vector<char> &body) {
        return RequestBody();
    }
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
    std::vector<std::pair<std::string, std::string> > headers_;
};

//------------------------------------------------------------------------------
class HttpResponse {
   public:
    HttpResponse() {}
    HttpResponse(const Response &r) : header_(r.headers()) {}
    ResponseHeader header() { return header_; }

   private:
    ResponseHeader header_;
    Response response_;
};

#endif
