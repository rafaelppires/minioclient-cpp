#include <httpclient.h>
#include <httpresponse.h>
#include <minioexceptions.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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
    char buffer[1024] = {0};
    int len = recv(sock, buffer, 1024, 0);
    if (len < 0) return empty;
    std::string input(buffer, len), headers;
    size_t hstart, hend;
    hstart = hend = std::string::npos;

    StatusLine sl = StatusLine::parse(input, &hstart);
    if (hstart != std::string::npos) hend = input.find("\r\n\r\n", hstart);
    if (hend != std::string::npos) {
        headers = trim_copy(input.substr(hstart, hend - hstart));
    }
    return ResponseBuilder()
        .protocol(sl.protocol_)
        .code(sl.code_)
        .message(sl.message_)
        .headers(HeadersBuilder::parse(headers))
        .build();
}

//------------------------------------------------------------------------------
// STATUS LINE
//------------------------------------------------------------------------------
StatusLine StatusLine::parse(const std::string &in, size_t *lastpos) {
    StatusLine ret;
    size_t pos = in.find('\n');
    if (pos != std::string::npos) {
        std::string statusline(trim_copy(in.substr(0, pos)));
        auto pieces = StringUtils::split(statusline, " ");
        if (pieces.size() < 2)
            throw IllegalStateException("Invalid status line: '" + statusline +
                                        "'");
        ret.protocol_ = pieces[0];
        ret.code_ = std::stoi(pieces[1]);
        if (pieces.size() > 2) {
            ret.message_ = Joiner::on(" ").join(
                std::vector<std::string>(pieces.begin() + 2, pieces.end()));
        }
    } else {
        throw std::runtime_error("StatusLine: illegal input");
    }

    if (lastpos != nullptr) *lastpos = pos;
    return ret;
}

//------------------------------------------------------------------------------
// CALL
//------------------------------------------------------------------------------
Call::Call(HttpClient &c, const Request &r) : client_(c), request_(r) {}

//------------------------------------------------------------------------------
Response Call::execute() { return client_.dispatch(*this); }

//------------------------------------------------------------------------------
// RESPONSE
//------------------------------------------------------------------------------
Response::Response() : code_(-1) {}

//------------------------------------------------------------------------------
Response::Response(const ResponseBuilder &builder) {
    // request_ = builder.request_;
    protocol_ = builder.protocol_;
    code_ = builder.code_;
    message_ = builder.message_;
    // handshake_ = builder.handshake_;
    headers_ = builder.headers_.build();
    body_ = builder.body_;
    // networkResponse_ = builder.networkResponse_;
    // cacheResponse_ = builder.cacheResponse_;
    // priorResponse_ = builder.priorResponse_;
    // sentRequestAtMillis_ = builder.sentRequestAtMillis_;
    // receivedResponseAtMillis_ = builder.receivedResponseAtMillis_;
}

//------------------------------------------------------------------------------
bool Response::empty() { return code_ == -1; }

//------------------------------------------------------------------------------
bool Response::isSuccessful() { return code_ >= 200 && code_ < 300; }

//------------------------------------------------------------------------------
const Headers &Response::headers() const { return headers_; }

//------------------------------------------------------------------------------
// RESPONSE BUILDER
//------------------------------------------------------------------------------
ResponseBuilder::ResponseBuilder(const Response &response) {
    // request_ = response.request_;
    protocol_ = response.protocol_;
    code_ = response.code_;
    message_ = response.message_;
    // handshake_ = response.handshake_;
    headers_ = response.headers_.newBuilder();
    body_ = response.body_;
    // networkResponse_ = response.networkResponse_;
    // cacheResponse_ = response.cacheResponse_;
    // priorResponse_ = response.priorResponse_;
    // sentRequestAtMillis_ = response.sentRequestAtMillis_;
    // receivedResponseAtMillis_ = response.receivedResponseAtMillis_;
}

//------------------------------------------------------------------------------
Response ResponseBuilder::build() {
    // if (request_.empty()) throw new IllegalStateException("request == null");
    if (protocol_.empty()) throw IllegalStateException("no protocol");
    if (code_ < 0)
        throw IllegalStateException("code < 0: " + std::to_string(code_));
    if (message_.empty()) throw IllegalStateException("no message");
    return Response(*this);
}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::protocol(const std::string &p) {
    protocol_ = p;
    return *this;
}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::code(int c) {
    code_ = c;
    return *this;
}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::message(const std::string &m) {
    message_ = m;
    return *this;
}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::headers(const HeadersBuilder &hb) {
    headers_ = hb;
    return *this;
}

//------------------------------------------------------------------------------
