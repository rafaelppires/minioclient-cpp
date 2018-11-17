#include <httprequest.h>

//------------------------------------------------------------------------------
// HEADERS
//------------------------------------------------------------------------------
Headers::Headers(const HeadersBuilder &hb) {
    headers_ = hb.headers_;
}

std::string Headers::get(const std::string &k) {
    for (const auto &it : headers_) {
        if (it.first == k) return it.second;
    }
    return "";
}

//------------------------------------------------------------------------------
HeadersBuilder Headers::newBuilder() const {
    HeadersBuilder ret(*this);
    return ret;
}

//------------------------------------------------------------------------------
std::string Headers::toString() {
    std::string ret;
    for (const auto &it : headers_) {
        ret += it.first + ": " + it.second + "\n";
    }
    return ret;
}

//------------------------------------------------------------------------------
// HEADERS BUILDER
//------------------------------------------------------------------------------
HeadersBuilder::HeadersBuilder(const Headers &h) { headers_ = h.headers_; }

//------------------------------------------------------------------------------
Headers HeadersBuilder::build() const { return Headers(*this); }

//------------------------------------------------------------------------------
HeadersBuilder &HeadersBuilder::set(const std::string &key,
                                    const std::string &value) {
    headers_.push_back(std::make_pair(key, value));
    return *this;
}

//------------------------------------------------------------------------------
// REQUEST
//------------------------------------------------------------------------------
Request::Request(const RequestBuilder &builder) {
    url_ = builder.url_;
    method_ = builder.method_;
    headers_ = builder.headers_.build();
    body_ = builder.body_;
    // tags = Util.immutableMap(builder.tags);
}

//------------------------------------------------------------------------------
// REQUEST BUILDER
//------------------------------------------------------------------------------
RequestBuilder::RequestBuilder() {}

//------------------------------------------------------------------------------
RequestBuilder::RequestBuilder(const Request &request) {
    url_ = request.url_;
    method_ = request.method_;
    body_ = request.body_;
    // tags = request.tags.isEmpty()
    //    ? Collections.<Class<?>, Object>emptyMap()
    //    : new LinkedHashMap<>(request.tags);
    headers_ = request.headers_.newBuilder();
}

//------------------------------------------------------------------------------
void RequestBuilder::url(const HttpUrl &url) { url_ = url; }

//------------------------------------------------------------------------------
void RequestBuilder::method(const std::string &method,
                            const RequestBody &body) {
    method_ = method;
    body_ = body;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::header(const std::string &key,
                                       const std::string &value) {
    headers_.set(key, value);
    return *this;
}

//------------------------------------------------------------------------------
Request RequestBuilder::build() {
    // if( url_ == null ) throw IllegalStateException("url == null");
    return Request(*this);
}

//------------------------------------------------------------------------------
