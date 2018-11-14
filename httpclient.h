#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <string>
#include <map>
#include <set>

class HttpUrl {
   public:
    std::string encodedPath() { return ""; }
    std::string encodedQuery() { return ""; }
    bool isHttps() const { return false; }
    int port() const { return port_; }
    std::string host() const { return ""; }

    static HttpUrl parse(const std::string &endpoint);

   private:
    int port_;
};

class Headers {
   public:
    std::string toString() { return ""; }
    operator const std::string() { return toString(); }
    std::string get(const std::string &key);
    std::set<std::string> names();

   private:
    typedef std::map<std::string,std::string> HeadersType;
    HeadersType headers_;
};

class Request {
   public:
    HttpUrl url() { return HttpUrl(); }
    std::string method() { return ""; }
    std::string header(const std::string &) { return ""; }
    Headers headers() { return Headers(); }

   private:
};

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

class Call {
   public:
    Response execute() { return Response(); }

   private:
};

class HttpClient {
   public:
    Call newCall(const Request &);

   private:
};

#endif
