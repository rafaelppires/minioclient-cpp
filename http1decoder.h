#ifndef _HTTP1_DECODER_H_
#define _HTTP1_DECODER_H_

#include <httpresponse.h>
#include <httprequest.h>
#include <tcpconnection.h>
#include <deque>
#include <string>

//------------------------------------------------------------------------------
class Http1Decoder {
   public:
    static const std::string crlf;
    Http1Decoder();
    void addChunk(const std::string &input);
    Response requestReply(EndpointConnection&, const Request &r);
    Response getResponse();
    Request getRequest();
    bool responseReady() const;
    bool requestReady() const;

   private:
    enum State { START, HEADER, BODY, CHUNKED };

    bool start_state();
    bool header_state();
    bool body_state();
    bool chunked_state();
    void reset();
    std::string stateString();

    State s_;
    std::string buffer_;
    std::deque<ResponseBuilder> responsequeue_;
    std::deque<RequestBuilder> requestqueue_;
    int content_len_;
    bool body_mustnot_, head_, request_;
    size_t decoded_messages_;
};

//------------------------------------------------------------------------------
class StatusLine {
   public:
    StatusLine() : code_(-1) {}
    static StatusLine parse(const std::string &, size_t *pos = nullptr);

   private:
    std::string protocol_, message_, uri_;
    bool request_;
    int code_;
    friend class Http1Decoder;
};

//------------------------------------------------------------------------------
#endif
