#include <httprequest.h>

//------------------------------------------------------------------------------
// REQUEST BODY
//------------------------------------------------------------------------------
RequestBody::RequestBody(const std::string &contentType,
                         std::vector<char> &&body) {
    data_ = body;
}
//------------------------------------------------------------------------------
RequestBody::RequestBody(const RequestBody &r)
    : data_(r.data_), content_type_(r.content_type_) {}

//------------------------------------------------------------------------------
RequestBody::RequestBody(RequestBody &&r)
    : data_(std::move(r.data_)), content_type_(std::move(r.content_type_)) {}

//------------------------------------------------------------------------------
RequestBody &RequestBody::operator=(const RequestBody &r) {
    if (this == &r) return *this;
    data_ = r.data_;
    content_type_ = r.content_type_;
    return *this;
}

//------------------------------------------------------------------------------
RequestBody &RequestBody::operator=(RequestBody &&r) {
    if (this == &r) return *this;
    data_ = std::move(r.data_);
    content_type_ = std::move(content_type_);
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
bool Request::hasBody() const {
    return !body_.data_.empty();
}

//------------------------------------------------------------------------------
const ByteArray &Request::body() const {
    return body_.data_;
}

//------------------------------------------------------------------------------
const HttpUrl &Request::url() const { return url_; }

//------------------------------------------------------------------------------
const Headers &Request::headers() const { return headers_; }

//------------------------------------------------------------------------------
std::string Request::method() const { return method_; }

//------------------------------------------------------------------------------
std::string Request::header(const std::string &key) const {
    return headers_.get(key);
}

//------------------------------------------------------------------------------
std::string Request::headerString() const { return headers_; }

//------------------------------------------------------------------------------
std::string Request::statusLine() const {
    std::string encodedPath = url_.encodedPath();
    std::string encodedQuery = url_.encodedQuery();
    if (!encodedQuery.empty()) {
        encodedPath += "?" + encodedQuery;
    }
    return method_ + " " + encodedPath + " HTTP/1.1";
}

//------------------------------------------------------------------------------
std::string Request::httpHeader() const {
    std::string content_length;
    content_length = "Content-Length: " + std::to_string(body_.data_.size());
    return statusLine() + "\n" + headerString() + content_length;
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
void RequestBuilder::method(const std::string &method, RequestBody &&body) {
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
