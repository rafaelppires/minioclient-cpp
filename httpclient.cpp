#include <httpclient.h>

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
std::string Headers::get(const std::string &k) {
    HeadersType::const_iterator it = headers_.find(k);
    if( it != headers_.end() )
        return it->second;
    return "";
}
//------------------------------------------------------------------------------

