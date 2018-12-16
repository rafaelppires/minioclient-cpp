#include <httpclient.h>
#include <httpresponse.h>
#include <minioexceptions.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef ENCLAVED
#undef connect
#endif

//------------------------------------------------------------------------------
// CONNECTION
//------------------------------------------------------------------------------
Connection::Connection() : connection_(nullptr) {}

//------------------------------------------------------------------------------
// Receives a EndpointConnection allocated with new. This transfers its
// ownership, meaning that the deletion happens in this class' destructor
Connection::Connection(EndpointConnection *c) : connection_(c) {}

//------------------------------------------------------------------------------
Connection::Connection(Connection &&c)
    : connection_(c.connection_), decoder_(std::move(c.decoder_)) {
    c.connection_ = nullptr;
}

//------------------------------------------------------------------------------
Connection &Connection::operator=(Connection &&c) {
    delete connection_;
    connection_ = c.connection_;
    c.connection_ = nullptr;
    decoder_ = std::move(c.decoder_);
    return *this;
}

//------------------------------------------------------------------------------
Connection::~Connection() { delete connection_; }

//------------------------------------------------------------------------------
Response Connection::requestReply(const Request &r) {
    if (r.method() == "HEAD") decoder_.setHead();
    std::string msg = r.toString();
    if (connection_->send(msg.data(), msg.size()) < 0)
        throw std::runtime_error("send: error " + std::to_string(errno));

    do {
        char buffer[1024] = {0};
        int len = connection_->recv(buffer, sizeof(buffer));
        if (len < 0) {
            throw std::runtime_error("recv: error " + std::to_string(errno));
        } else if (len == 0) {
            throw std::runtime_error("remote: closed connection ");
        }
        decoder_.addChunk(std::string(buffer, len));
    } while (!decoder_.responseReady());

    return decoder_.getResponse();
}

//------------------------------------------------------------------------------
// HTTP CLIENT
//------------------------------------------------------------------------------
HttpClient::HttpClient() {}

//------------------------------------------------------------------------------
HttpClient::~HttpClient() {}

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
            auto ins = connections_.insert(
                std::make_pair(url.baseUrl(), Connection(endpoint)));
            it = ins.first;
        } else {
            return empty;
        }
    }

    try {
        return it->second.requestReply(call.request_);
    } catch (const std::runtime_error &e) {
        std::string msg(e.what());
        if (msg.find("send:") == 0 || msg.find("recv:") == 0 ||
            msg.find("remote:") == 0) {
            connections_.erase(it);
            return empty;
        } else
            throw e;
    }
}

//------------------------------------------------------------------------------
// CALL
//------------------------------------------------------------------------------
Call::Call(HttpClient &c, const Request &r) : client_(c), request_(r) {}

//------------------------------------------------------------------------------
Response Call::execute() { return client_.dispatch(*this); }

//------------------------------------------------------------------------------
