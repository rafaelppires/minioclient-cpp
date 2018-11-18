#include <httpheaders.h>
#include <stringutils.h>
using namespace StringUtils;
//------------------------------------------------------------------------------
// HEADERS
//------------------------------------------------------------------------------
const std::string Headers::eol = "\r\n";
//------------------------------------------------------------------------------
Headers::Headers(const HeadersBuilder &hb) { headers_ = hb.headers_; }

//------------------------------------------------------------------------------
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
std::string Headers::toString() const {
    std::string ret;
    for (const auto &it : headers_) {
        ret += it.first + ": " + it.second + eol;
    }
    return ret;
}

//------------------------------------------------------------------------------
std::set<std::string> Headers::names() {
    std::set<std::string> ret;
    for (const auto &kv : headers_) {
        ret.insert(kv.first);
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
HeadersBuilder HeadersBuilder::parse(const std::string &input) {
    HeadersBuilder ret;
    for (const auto &kv : split(input, Headers::eol)) {
        auto kv_split = split(kv, ":");
        if (kv_split.size() >= 2)
            ret.set(trim_copy(kv_split[0]),
                    trim_copy(Joiner::on(":").join(std::vector<std::string>(
                        kv_split.begin() + 1, kv_split.end()))));
    }
    return ret;
}
