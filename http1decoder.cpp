#include <http1decoder.h>
#include <minioexceptions.h>
#include <sys/socket.h>

//------------------------------------------------------------------------------
// HTTP1 DECODER
//------------------------------------------------------------------------------
const std::string Http1Decoder::crlf = "\r\n";
//------------------------------------------------------------------------------
Http1Decoder::Http1Decoder() : s_(START), head_(false) {}

//------------------------------------------------------------------------------
Response Http1Decoder::requestReply(int sock, const Request& r) {
    if (r.method() == "HEAD") head_ = true;
    std::string msg = r.httpHeader() + crlf + crlf;
    send(sock, msg.data(), msg.size(), 0);
    if (r.hasBody()) {
        const auto& b = r.body();
        send(sock, b.data(), b.size(), 0);
    }

    do {
        char buffer[1024] = {0};
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0)
            throw std::runtime_error("IO error");
        else if (len == 0)
            throw std::runtime_error("Remote closed connection");
        addChunk(std::string(buffer, len));
    } while (!ready());

    return get();
}

//------------------------------------------------------------------------------
void Http1Decoder::addChunk(const std::string& input) {
    buffer_ += input;
    //printf(">>>>>>>>>>>>%s<<<<<<<<<<<<\n", buffer_.c_str());
    switch (s_) {
        case START:
            if (start_state()) break;
        case HEADER:
            if (header_state()) break;
        case BODY:
            if (body_state()) break;
        case CHUNKED:
            if (chunked_state()) break;
        default:
            throw IllegalStateException("HTTP Decoder unknown state: " +
                                        std::to_string(s_));
    };
}

//------------------------------------------------------------------------------
bool Http1Decoder::start_state() {
    s_ = HEADER;  // potentially avoid concurrency issues with ready()
    readyqueue_.emplace(readyqueue_.end());
    ResponseBuilder& response = readyqueue_.back();
    size_t headerstart;
    StatusLine sl = StatusLine::parse(buffer_, &headerstart);
    response.protocol(sl.protocol_).code(sl.code_).message(sl.message_);
    buffer_.erase(0, headerstart);

    body_mustnot_ = head_ || (sl.code_ >= 100 && sl.code_ < 199) ||
                    sl.code_ == 204 || sl.code_ == 304;
    content_len_ = -1;

    return buffer_.empty();
}

//------------------------------------------------------------------------------
bool Http1Decoder::header_state() {
    size_t headerend = buffer_.find(crlf + crlf);
    if (headerend == std::string::npos) {
        return true;  // incomplete, wait more data
    }
    ResponseBuilder& response = readyqueue_.back();
    response.headers(HeadersBuilder::parse(buffer_.substr(0, headerend)));
    buffer_.erase(0, headerend + 4);

    if (body_mustnot_) {
        reset();
        return true;  // finished
    }

    if (tolower(response.get("Connection")) == "close") {
        content_len_ = 0;
        s_ = BODY;
        return false;  // keep reading the body
    }

    if (tolower(response.get("Transfer-Encoding")) == "chunked") {
        s_ = CHUNKED;
        content_len_ = 0;
        addChunk("");
        return true;  // it's a chunked message, skip BODY case
    } else {
        s_ = BODY;
        return false; // Content-Length may be 0 (in case buffer_ may be empty)
    }

    return buffer_.empty();
}
//------------------------------------------------------------------------------
bool Http1Decoder::body_state() {
    ResponseBuilder& response = readyqueue_.back();
    if (content_len_ < 0) {
        std::string lenstr = response.get("Content-Length");
        if (response.get("Connection") == "close") return true;
        if (lenstr.empty())
            throw IllegalStateException(
                "Either 'Transfer-Encoding: chunked', 'Content-Length: ' or "
                "'Connection: close' must be provided for code != 1xx 204 or "
                "304");
        content_len_ = std::stoi(lenstr);
        if (content_len_ == 0) {
             reset();
        }
    } else if (content_len_ == 0 &&
               !buffer_.empty()) {  // single shot due to Connection: close
        response.addBody(buffer_);
        buffer_.clear();
        reset();
    }
    return true;
}

//------------------------------------------------------------------------------
bool Http1Decoder::chunked_state() {
    size_t pos = buffer_.find(crlf);
    if (pos == std::string::npos) return true;

    unsigned size;
    sscanf(buffer_.substr(0, pos).c_str(), "%x", &size);
    buffer_.erase(0, pos + crlf.size());
    if (size == 0) {
        reset();
        return true;
    }

    content_len_ += size;

    pos = buffer_.find(crlf);
    if (pos == std::string::npos) return true;

    std::string chunk = buffer_.substr(0, pos);
    if (content_len_ >= chunk.size()) {
        ResponseBuilder& response = readyqueue_.back();
        response.addBody(chunk);
        buffer_.erase(0, pos + crlf.size());
        content_len_ -= chunk.size();
    }
    return true;
}

//------------------------------------------------------------------------------
void Http1Decoder::reset() {
    head_ = false;
    s_ = START;
}

//------------------------------------------------------------------------------
bool Http1Decoder::ready() const {
    if (readyqueue_.size() > 1)
        return true;
    else if (!readyqueue_.empty())
        return s_ == START;
    return false;
}

//------------------------------------------------------------------------------
Response Http1Decoder::get() {
    if (!ready()) return Response();
    ResponseBuilder ret;
    std::swap(ret, readyqueue_.front());
    readyqueue_.pop_front();
    return ret.build();
}

//------------------------------------------------------------------------------
// STATUS LINE
//------------------------------------------------------------------------------
StatusLine StatusLine::parse(const std::string& in, size_t* lastpos) {
    StatusLine ret;
    size_t pos = in.find('\n');
    if (pos != std::string::npos) {
        std::string statusline(trim_copy(in.substr(0, pos)));
        auto pieces = StringUtils::split(statusline, " ");
        if (pieces.size() < 3)
            throw IllegalStateException("Invalid status line: '" + statusline +
                                        "'");
        ret.protocol_ = pieces[0];
        ret.code_ = std::stoi(pieces[1]);
        ret.message_ = Joiner::on(" ").join(
            std::vector<std::string>(pieces.begin() + 2, pieces.end()));
    } else {
        throw std::runtime_error("StatusLine: illegal input");
    }

    if (lastpos != nullptr) *lastpos = pos;
    return ret;
}

//------------------------------------------------------------------------------
