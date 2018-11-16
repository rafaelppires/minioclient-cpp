#include <httpurl.h>
#include <minioexceptions.h>
#include <stringutils.h>
using namespace StringUtils;
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
    // this.url = builder.toString();
}

//------------------------------------------------------------------------------
HttpUrl HttpUrl::parse(const std::string &endpoint) {
    UrlBuilder builder = UrlBuilder::parse(endpoint);
    return builder.build();
}
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
UrlBuilder::UrlBuilder() : port_(-1) {}

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
UrlBuilder UrlBuilder::parse(const std::string &input) {
    UrlBuilder ret;
    auto slices = split(input, "/");
    if (!slices.empty()) {
        ret.scheme_ = slices[0].substr(0, slices[0].size() - 1);
        if (slices[0] != "http:" && slices[0] != "https:") {
            throw std::invalid_argument(
                "Expected URL scheme 'http' or 'https' but was '" +
                ret.scheme_ + "'");
        }
    }
    if (slices.size() > 1) {
        auto endpoint_slices = split(slices[1], ":");
        if (endpoint_slices.empty()) {
            throw std::invalid_argument("No hostname");
        } else {
            ret.host(endpoint_slices[0]);
            if (endpoint_slices.size() > 1)
                ret.port(std::stoi(endpoint_slices[1]));
        }
    } else {
        throw std::invalid_argument("No hostname");
    }
    return ret;
}

//------------------------------------------------------------------------------
void UrlBuilder::addEncodedPathSegment(const std::string &ps) {
    path_segments_.push_back(ps);
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
void UrlBuilder::url(const HttpUrl &) {}

//------------------------------------------------------------------------------
void UrlBuilder::addPathSegment(const std::string &ps) {
    path_segments_.push_back(ps);
}

//------------------------------------------------------------------------------
