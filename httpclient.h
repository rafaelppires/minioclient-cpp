#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <httprequest.h>
#include <map>
#include <set>
#include <string>

//------------------------------------------------------------------------------
class ResponseBuilder;
class Response {
   public:
    Response();
    Response(const ResponseBuilder &);
    bool empty();
    bool isSuccessful();
    const Headers &headers() const;
    std::string protocol() { return protocol_; }
    std::string body() { return body_; }
    int code() { return code_; }

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
    ResponseBuilder(const Response &);
    ResponseBuilder &protocol(const std::string &);
    ResponseBuilder &code(int);
    ResponseBuilder &message(const std::string &);
    ResponseBuilder &headers(const HeadersBuilder &);
    Response build();

   private:
    int code_;
    std::string body_, protocol_, message_;
    HeadersBuilder headers_;
    friend class Response;
};

//------------------------------------------------------------------------------
class Call;
class HttpClient {
   public:
    HttpClient();
    Call newCall(const Request &);
    Response dispatch(Call &);

   private:
    std::map<std::string, int> connections_;

    int connect(const std::string host, int port);
};

//------------------------------------------------------------------------------
class StatusLine {
   public:
    StatusLine() : code_(-1) {}
    static StatusLine parse(const std::string &, size_t *pos = nullptr);

   private:
    std::string protocol_, message_;
    int code_;
    friend class HttpClient;
};

//------------------------------------------------------------------------------
class Call {
   public:
    Call(HttpClient &c, const Request &r);
    Response execute();

   private:
    Request request_;
    HttpClient &client_;
    friend class HttpClient;
};

#endif
