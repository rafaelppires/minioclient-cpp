#include <httpurl.h>
#include <minioexceptions.h>
#include <stringutils.h>
using namespace StringUtils;
//------------------------------------------------------------------------------
// ESCAPER
//------------------------------------------------------------------------------
std::string Escaper::encode(std::string str) {
    return str = replaceAll(str, " ", "%20");
    str = replaceAll(str, "!", "%21");
    str = replaceAll(str, "$", "%24");
    str = replaceAll(str, "&", "%26");
    str = replaceAll(str, "'", "%27");
    str = replaceAll(str, "(", "%28");
    str = replaceAll(str, ")", "%29");
    str = replaceAll(str, "*", "%2A");
    str = replaceAll(str, "+", "%2B");
    str = replaceAll(str, ",", "%2C");
    str = replaceAll(str, "/", "%2F");
    str = replaceAll(str, ":", "%3A");
    str = replaceAll(str, ";", "%3B");
    str = replaceAll(str, "=", "%3D");
    str = replaceAll(str, "@", "%40");
    str = replaceAll(str, "[", "%5B");
    str = replaceAll(str, "]", "%5D");
    return str;
}

//------------------------------------------------------------------------------
// SCHEME
//------------------------------------------------------------------------------
std::string Scheme::toString() {
    switch (s_) {
        case HTTP:
            return "http";
        case HTTPS:
            return "https";
        default:
            return "";
    };
}

//------------------------------------------------------------------------------
// HTTP URL
//------------------------------------------------------------------------------
HttpUrl::HttpUrl(const UrlBuilder &builder) {
    scheme_ = builder.scheme_;
    // this.username = percentDecode(builder.encodedUsername, false);
    // this.password = percentDecode(builder.encodedPassword, false);
    host_ = builder.host_;
    port_ = builder.effectivePort();
    // this.pathSegments = percentDecode(builder.encodedPathSegments, false);
    // this.queryNamesAndValues = builder.encodedQueryNamesAndValues != null
    //    ? percentDecode(builder.encodedQueryNamesAndValues, true)
    //    : null;
    // this.fragment = builder.encodedFragment != null
    //    ? percentDecode(builder.encodedFragment, false)
    //    : null;
    url_ = builder.toString();
}

//------------------------------------------------------------------------------
HttpUrl HttpUrl::parse(const std::string &endpoint) {
    UrlBuilder builder = UrlBuilder::parse(endpoint);
    return builder.build();
}

//------------------------------------------------------------------------------
std::string HttpUrl::encodedPath() const {
    size_t pos = url_.find("/", scheme_.size() + 3);
    if (pos == std::string::npos) 
        return "";
    return url_.substr(pos);
}

//------------------------------------------------------------------------------
std::string HttpUrl::baseUrl() const {
    return scheme_ + "://" + host() + ":" + std::to_string(port_);
}

//------------------------------------------------------------------------------
std::string HttpUrl::encodedQuery() const { return ""; }

//------------------------------------------------------------------------------
UrlBuilder HttpUrl::newBuilder() {
    UrlBuilder ret;
    ret.host(host_).scheme(scheme_).port(
        port_ != UrlBuilder::defaultPort(scheme_) ? port_ : -1);
    return ret;
}

//------------------------------------------------------------------------------
// URL BUILDER
//------------------------------------------------------------------------------
UrlBuilder::UrlBuilder() : port_(-1) { path_segments_.push_back(""); }

//------------------------------------------------------------------------------
std::string UrlBuilder::toString() const {
    std::string ret;
    if (scheme_.empty())
        ret += "//";
    else
        ret += scheme_ + "://";
    ret += host_;
    if (port_ != -1 || !scheme_.empty()) {
        int p = effectivePort();
        if (scheme_.empty() || p != defaultPort(scheme_))
            ret += ":" + std::to_string(p);
    }
    for (const auto &seg : path_segments_) ret += "/" + seg;
    return ret;
}

//------------------------------------------------------------------------------
UrlBuilder &UrlBuilder::host(const std::string &host) {
    host_ = host;
    return *this;
}

//------------------------------------------------------------------------------
UrlBuilder &UrlBuilder::port(int p) {
    if (p <= 0 && p > 65535)
        throw std::invalid_argument("unexpected port: " + std::to_string(p));
    port_ = p;
    return *this;
}

//------------------------------------------------------------------------------
UrlBuilder &UrlBuilder::scheme(const std::string &s) {
    scheme_ = s;
    return *this;
}

//------------------------------------------------------------------------------
int UrlBuilder::defaultPort(const std::string &scheme) {
    return scheme == "https" ? 443 : (scheme == "http" ? 80 : -1);
}

//------------------------------------------------------------------------------
extern "C" { int printf(const char *,...); }
UrlBuilder UrlBuilder::parse(const std::string &input) {
    UrlBuilder ret;
    auto slices = split(input, "/");
    if (slices.empty()) return ret;
    int pos = 0;
    if (input.find("://") != std::string::npos ) {
        ret.scheme_ = slices[pos].substr(pos, slices[pos].size() - 1);
        if (slices[pos] != "http:" && slices[pos] != "https:") {
            throw std::invalid_argument(
                "Expected URL scheme 'http' or 'https' but was '" +
                ret.scheme_ + "'");
        }
        ++pos;
        if (slices.size() > 1) {
            auto endpoint_slices = split(slices[pos], ":");
            if (endpoint_slices.empty()) {
                throw std::invalid_argument("No hostname");
            } else {
                ret.host(endpoint_slices[0]);
                if (endpoint_slices.size() > 1) // else standard port
                    ret.port(std::stoi(endpoint_slices[1]));
            }
            ++pos;
        } else {
            throw std::invalid_argument("No hostname");
        }
    }

    if( pos < slices.size() )
        ret.path_segments_.clear();

    for(; pos < slices.size(); ++pos)
        ret.path_segments_.push_back(slices[pos]);

    return ret;
}

//------------------------------------------------------------------------------
void UrlBuilder::addEncodedQueryParameter(const std::string &key,
                                          const std::string &value) {
    encoded_query_params_.push_back(std::make_pair(key, value));
}

//------------------------------------------------------------------------------
int UrlBuilder::effectivePort() const {
    return port_ != -1 ? port_ : defaultPort(scheme_);
}

//------------------------------------------------------------------------------
HttpUrl UrlBuilder::build() {
    if (host_.empty()) throw IllegalStateException("no host defined");
    if (scheme_.empty())
        throw IllegalStateException("no scheme defined (http or https)");
    return HttpUrl(*this);
}

//------------------------------------------------------------------------------
void UrlBuilder::url(const HttpUrl &url) {
}

//------------------------------------------------------------------------------
void UrlBuilder::addPathSegment(const std::string &ps) {
    addEncodedPathSegment(Escaper::encode(ps));
}

//------------------------------------------------------------------------------
void UrlBuilder::addEncodedPathSegment(const std::string &ps) {
    auto &l = path_segments_;
    //l.insert(l.begin() + (l.empty() ? 0 : l.size() - 1), ps);
    if( l.size() == 1 && l.front() == "" )
        l.clear();
    l.push_back(ps);
}

//------------------------------------------------------------------------------
