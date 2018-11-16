#include <httpclient.h>
#include <httpresponse.h>

//------------------------------------------------------------------------------
// HTTP CLIENT
//------------------------------------------------------------------------------
Call HttpClient::newCall(const Request &) {
    Call ret;
    return ret;
}

//------------------------------------------------------------------------------
std::set<std::string> Headers::names() {
    std::set<std::string> ret;
    for( const auto &kv : headers_ ) {
        ret.insert( kv.first );
    }
    return ret;
}

//------------------------------------------------------------------------------
// HEADERS
//------------------------------------------------------------------------------
std::string Headers::get(const std::string &k) {
    HeadersType::const_iterator it = headers_.find(k);
    if( it != headers_.end() )
        return it->second;
    return "";
}
//------------------------------------------------------------------------------
// RESPONSE
//------------------------------------------------------------------------------
bool Response::isSuccessful() {
    return true;
}

//------------------------------------------------------------------------------
Headers Response::headers() const {
    return Headers();
}

