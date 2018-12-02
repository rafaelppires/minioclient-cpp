#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <httprequest.h>
#include <httpresponse.h>
#include <http1decoder.h>
#include <map>
#include <string>

//------------------------------------------------------------------------------
class Connection {
public:
    Connection();
    Connection(EndpointConnection *);
    Connection(Connection &&c);
    Connection& operator=(Connection &&c);
    ~Connection();
    Response requestReply(const Request& r);
private:
    EndpointConnection *connection_;
    Http1Decoder decoder_;
};

//------------------------------------------------------------------------------
class Call;
class HttpClient {
   public:
    HttpClient();
    HttpClient(HttpClient&&) = default;
    HttpClient& operator=(HttpClient&&) = default;
    ~HttpClient();
    Call newCall(const Request &);
    Response dispatch(Call &);

   private:
    std::map<std::string, Connection> connections_;
    Http1Decoder decoder_;
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
