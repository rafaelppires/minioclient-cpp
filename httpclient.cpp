#include <httpclient.h>
#include <httpresponse.h>
#include <minioexceptions.h>

#ifndef ENCLAVED
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#include <inet_pton_ntop.h>
#include <my_wrappers.h>
#define htons(n) \
    (((((unsigned short)(n)&0xFF)) << 8) | (((unsigned short)(n)&0xFF00) >> 8))
#endif

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
int HttpClient::connect(const std::string &host, int port) {
    struct sockaddr_in address;
    int sock = 0;

    struct addrinfo hints;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res, *it;
    int errorcode;
    if ((errorcode = getaddrinfo(host.c_str(), std::to_string(port).c_str(),
                                 &hints, &res))) {
        printf("Address resolution error %d: '%s'\n", errorcode, host.c_str());
        return -1;
    }

    for (it = res; it != nullptr; it = it->ai_next) {
        if ((sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol)) <
            0) {
            printf("Error getting socket: %d\n", errno);
            continue;
        }

        if (::connect(sock, it->ai_addr, it->ai_addrlen) < 0) {
            printf("Connection failed: %d\n", errno);
            continue;
        }
        break;
    }

    return it == nullptr ? -2 : sock;
}

//------------------------------------------------------------------------------
Call HttpClient::newCall(const Request &r) { return Call(*this, r); }

//------------------------------------------------------------------------------
Response HttpClient::dispatch(Call &call) {
    Response empty;
    int sock;
    auto it = connections_.end();
    const HttpUrl &url = call.request_.url();
    if ((it = connections_.find(url.toString())) == connections_.end()) {
        if ((sock = connect(url.host(), url.port())) > 0)
            connections_[url.toString()] = sock;
        else
            return empty;
    } else {
        sock = it->second;
    }

    return decoder_.requestReply(sock, call.request_);
}

//------------------------------------------------------------------------------
// CALL
//------------------------------------------------------------------------------
Call::Call(HttpClient &c, const Request &r) : client_(c), request_(r) {}

//------------------------------------------------------------------------------
Response Call::execute() { return client_.dispatch(*this); }

//------------------------------------------------------------------------------
