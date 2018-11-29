#include <httpclient.h>
#include <httpresponse.h>
#include <minioexceptions.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// HTTP CLIENT
//------------------------------------------------------------------------------
HttpClient::HttpClient() {}

//------------------------------------------------------------------------------
Call HttpClient::newCall(const Request &r) { return Call(*this, r); }

//------------------------------------------------------------------------------
Response HttpClient::dispatch(Call &call) {
    Response empty;
    int sock;
    auto it = connections_.end();
    const HttpUrl &url = call.request_.url();
    EndpointConnection *endpoint;
    if ((it = connections_.find(url.baseUrl())) == connections_.end()) {
        if (EndpointConnection::connect(&endpoint, url)) {
            connections_[url.baseUrl()] = endpoint;
        } else {
            return empty;
        }
    } else {
        endpoint = it->second;
    }

    return decoder_.requestReply(*endpoint, call.request_);
}

//------------------------------------------------------------------------------
// CALL
//------------------------------------------------------------------------------
Call::Call(HttpClient &c, const Request &r) : client_(c), request_(r) {}

//------------------------------------------------------------------------------
Response Call::execute() { return client_.dispatch(*this); }

//------------------------------------------------------------------------------
