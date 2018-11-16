#include <httpresponse.h>

//------------------------------------------------------------------------------
// REQUEST BUILDER
//------------------------------------------------------------------------------
RequestBuilder::RequestBuilder() {}

//------------------------------------------------------------------------------
RequestBuilder::RequestBuilder(const Request &) {}

//------------------------------------------------------------------------------
void RequestBuilder::url(const HttpUrl &) {}

//------------------------------------------------------------------------------
void RequestBuilder::method(const std::string &method,
                            const RequestBody &body) {}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::header(const std::string &key,
                                       const std::string &value) {
    headers_.push_back(std::make_pair(key, value));
    return *this;
}

//------------------------------------------------------------------------------
Request RequestBuilder::build() { return Request(); }

//------------------------------------------------------------------------------
// RESPONSE HEADER
//------------------------------------------------------------------------------
ResponseHeader::ResponseHeader() {}
//------------------------------------------------------------------------------
ResponseHeader::ResponseHeader(const Headers &) {}
//------------------------------------------------------------------------------

