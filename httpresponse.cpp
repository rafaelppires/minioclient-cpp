#include <httpresponse.h>
#include <minioexceptions.h>

//------------------------------------------------------------------------------
// RESPONSE
//------------------------------------------------------------------------------
Response::Response() : code_(-1) {}
//------------------------------------------------------------------------------
Response::Response(Response &&r)
    : code_(std::move(r.code_)),
      body_(std::move(r.body_)),
      protocol_(std::move(r.protocol_)),
      message_(std::move(r.message_)),
      headers_(std::move(r.headers_)) {}

//------------------------------------------------------------------------------
Response &Response::operator=(Response &&r) {
    if (this == &r) return *this;
    code_ = std::move(r.code_);
    body_ = std::move(r.body_);
    protocol_ = std::move(r.protocol_);
    message_ = std::move(r.message_);
    headers_ = std::move(r.headers_);
    return *this;
}

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
ResponseBuilder::ResponseBuilder(ResponseBuilder &&r)
    : code_(std::move(r.code_)),
      body_(std::move(r.body_)),
      protocol_(std::move(r.protocol_)),
      message_(std::move(r.message_)),
      headers_(std::move(r.headers_)) {}

//------------------------------------------------------------------------------
ResponseBuilder::ResponseBuilder(const ResponseBuilder &r)
    : code_(r.code_),
      body_(r.body_),
      protocol_(r.protocol_),
      message_(r.message_),
      headers_(r.headers_) {}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::operator=(ResponseBuilder &&r) {
    if (this == &r) return *this;
    code_ = std::move(r.code_);
    body_ = std::move(r.body_);
    protocol_ = std::move(r.protocol_);
    message_ = std::move(r.message_);
    headers_ = std::move(r.headers_);
    return *this;
}

//------------------------------------------------------------------------------
ResponseBuilder &ResponseBuilder::operator=(const ResponseBuilder &r) {
    if (this == &r) return *this;
    code_ = r.code_;
    body_ = r.body_;
    protocol_ = r.protocol_;
    message_ = r.message_;
    headers_ = r.headers_;
    return *this;
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
ResponseBuilder &ResponseBuilder::appendBody(const std::string &chunk) {
    body_ += chunk;
    return *this;
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
std::string ResponseBuilder::getHeaderValue(const std::string &k) {
    return headers_.get(k);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// RESPONSE HEADER
//------------------------------------------------------------------------------
ResponseHeader::ResponseHeader() {}
//------------------------------------------------------------------------------
ResponseHeader::ResponseHeader(const Headers &) {}

//------------------------------------------------------------------------------
// HTTP RESPONSE
//------------------------------------------------------------------------------
HttpResponse::HttpResponse(HttpResponse &&r)
    : header_(std::move(r.header_)), response_(std::move(r.response_)) {}

//------------------------------------------------------------------------------
HttpResponse &HttpResponse::operator=(HttpResponse &&r) {
    if (this == &r) return *this;
    header_ = std::move(r.header_);
    response_ = std::move(r.response_);
    return *this;
}

//------------------------------------------------------------------------------
