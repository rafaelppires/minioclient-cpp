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
void RequestBody::append(const std::string &content) {
    data_.insert(data_.end(), content.begin(), content.end());
}

//------------------------------------------------------------------------------
// REQUEST
//------------------------------------------------------------------------------
Request::Request(const RequestBuilder &builder)  {
    url_ = builder.url_;
    method_ = builder.method_;
    headers_ = builder.headers_.build();
    body_ = builder.body_;
    protocol_ = builder.protocol_;
    // tags = Util.immutableMap(builder.tags);
}

//------------------------------------------------------------------------------
bool Request::hasBody() const { return !body_.data_.empty(); }

//------------------------------------------------------------------------------
const ByteArray &Request::body() const { return body_.data_; }

//------------------------------------------------------------------------------
std::string Request::stringBody() const {
     return std::string(body_.data_.begin(), body_.data_.end());
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
    return method_ + " " + encodedPath + " " + protocol_;
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
RequestBuilder::RequestBuilder() : protocol_("HTTP/1.1") {}

//------------------------------------------------------------------------------
RequestBuilder::RequestBuilder(const Request &request) {
    url_ = request.url_;
    method_ = request.method_;
    body_ = request.body_;
    protocol_ = request.protocol_;
    // tags = request.tags.isEmpty()
    //    ? Collections.<Class<?>, Object>emptyMap()
    //    : new LinkedHashMap<>(request.tags);
    headers_ = request.headers_.newBuilder();
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::url(const HttpUrl &url) {
    url_ = url;
    return *this;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::method(const std::string &method) {
    method_ = method;
    return *this;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::protocol(const std::string &p) {
    protocol_ = p;
    return *this;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::headers(const HeadersBuilder &h) {
    headers_ = h;
    return *this;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::body(RequestBody &&body) {
    body_ = body;
    return *this;
}

//------------------------------------------------------------------------------
RequestBuilder &RequestBuilder::appendBody(const std::string &content) {
    body_.append(content);
    return *this;
}

//------------------------------------------------------------------------------
std::string RequestBuilder::getHeaderValue(const std::string &k) {
    return headers_.get(k);
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
