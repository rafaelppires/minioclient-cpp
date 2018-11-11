#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <string>

class HttpUrl {
   public:
    std::string encodedPath();
    std::string encodedQuery();
    bool isHttps() const;
    unsigned port() const;
    std::string host() const;

    static HttpUrl parse(const std::string &endpoint);
   private:
};

class Headers {
   public:
    std::string toString();
    operator const std::string() { return toString(); }

   private:
};

class Request {
   public:
    HttpUrl url();
    std::string method();
    Headers headers();

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
    Response execute();

   private:
};

class HttpClient {
   public:
    Call newCall(const Request &);

   private:
};

#endif
