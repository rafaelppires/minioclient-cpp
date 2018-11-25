#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <httprequest.h>
#include <httpresponse.h>
#include <http1decoder.h>
#include <map>
#include <string>

//------------------------------------------------------------------------------
class Call;
class HttpClient {
   public:
    HttpClient();
    Call newCall(const Request &);
    Response dispatch(Call &);

   private:
    std::map<std::string, int> connections_;
    Http1Decoder decoder_;

    int connect(const std::string &host, int port);
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
