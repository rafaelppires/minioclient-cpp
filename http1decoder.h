#ifndef _HTTP1_DECODER_H_
#define _HTTP1_DECODER_H_

#include <httpresponse.h>
#include <httprequest.h>
#include <deque>
#include <string>

//------------------------------------------------------------------------------
class Http1Decoder {
   public:
    static const std::string crlf;
    Http1Decoder();
    void addChunk(const std::string &input);
    Response requestReply(int s, const Request &r);

   private:
    enum State { START, HEADER, BODY, CHUNKED };

    bool start_state();
    bool header_state();
    bool body_state();
    bool chunked_state();
    void reset();
    Response get();
    bool ready() const;

    State s_;
    std::string buffer_;
    std::deque<ResponseBuilder> readyqueue_;
    int content_len_;
    bool body_mustnot_, head_;
};

//------------------------------------------------------------------------------
class StatusLine {
   public:
    StatusLine() : code_(-1) {}
    static StatusLine parse(const std::string &, size_t *pos = nullptr);

   private:
    std::string protocol_, message_;
    int code_;
    friend class Http1Decoder;
};

//------------------------------------------------------------------------------
#endif
