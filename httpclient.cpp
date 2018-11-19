#include <httpclient.h>
#include <httpresponse.h>
#include <minioexceptions.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// HTTP CLIENT
//------------------------------------------------------------------------------
HttpClient::HttpClient() {}

//------------------------------------------------------------------------------
int HttpClient::connect(const std::string host, int port) {
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    } else {
        return sock;
    }
}

//------------------------------------------------------------------------------
Call HttpClient::newCall(const Request &r) { return Call(*this, r); }

//------------------------------------------------------------------------------
Response HttpClient::dispatch(Call &call) {
    Response empty;
    Request &r = call.request_;
    const HttpUrl &url = r.url();

    int sock;
    auto it = connections_.end();
    if ((it = connections_.find(url.toString())) == connections_.end()) {
        if ((sock = connect(url.host(), url.port())) > 0)
            connections_[url.toString()] = sock;
        else
            return empty;
    } else {
        sock = it->second;
    }

    std::string msg = r.httpHeader() + "\r\n\r\n";
    send(sock, msg.data(), msg.size(), 0);

    if (r.method() == "HEAD") decoder_.setHead();
    do {
        char buffer[1024] = {0};
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0)
            throw std::runtime_error("IO error");
        else if (len == 0)
            throw std::runtime_error("Remote closed connection");
        decoder_.addChunk(std::string(buffer, len));
    } while (!decoder_.ready());

    return decoder_.get();
}

//------------------------------------------------------------------------------
// CALL
//------------------------------------------------------------------------------
Call::Call(HttpClient &c, const Request &r) : client_(c), request_(r) {}

//------------------------------------------------------------------------------
Response Call::execute() { return client_.dispatch(*this); }

//------------------------------------------------------------------------------
