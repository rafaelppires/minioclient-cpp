#include <httprequest.h>

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
const HttpUrl &Request::url() { return url_; }

//------------------------------------------------------------------------------
const Headers &Request::headers() { return headers_; }

//------------------------------------------------------------------------------
std::string Request::method() { return method_; }

//------------------------------------------------------------------------------
std::string Request::header(const std::string &key) {
    return headers_.get(key);
}

//------------------------------------------------------------------------------
std::string Request::headerString() { return headers_; }

//------------------------------------------------------------------------------
std::string Request::statusLine() {
    std::string encodedPath = url_.encodedPath();
    std::string encodedQuery = url_.encodedQuery();
    if (!encodedQuery.empty()) {
        encodedPath += "?" + encodedQuery;
    }
    return method_ + " " + encodedPath + " HTTP/1.1";
}
//------------------------------------------------------------------------------
std::string Request::httpHeader() {
    return statusLine() + "\n" + headerString();
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
