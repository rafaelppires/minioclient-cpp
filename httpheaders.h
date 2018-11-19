#ifndef _HTTP_HEADERS_H_
#define _HTTP_HEADERS_H_

#include <set>
#include <list>
#include <string>

//------------------------------------------------------------------------------
typedef std::list<std::pair<std::string,std::string> > HeadersType;
class HeadersBuilder;
class Headers {
   public:
    static const std::string eol;
    Headers() {}
    Headers(const HeadersBuilder &);
    std::string toString() const;
    operator std::string() const { return toString(); }
    std::string get(const std::string &key);
    std::set<std::string> names();
    HeadersBuilder newBuilder() const;

   private:
    HeadersType headers_;
    friend class HeadersBuilder;
};

//------------------------------------------------------------------------------
class HeadersBuilder {
public:
    HeadersBuilder() {}
    HeadersBuilder(const Headers&);
    Headers build() const;
    std::string get(const std::string &key);
    HeadersBuilder &set(const std::string &key, const std::string &value);

    static HeadersBuilder parse(const std::string &);
private:
    HeadersType headers_;
    friend class Headers;
};

//------------------------------------------------------------------------------

#endif

