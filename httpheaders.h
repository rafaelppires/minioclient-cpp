#ifndef _HTTP_HEADERS_H_
#define _HTTP_HEADERS_H_

#include <list>
#include <set>
#include <string>

//------------------------------------------------------------------------------
typedef std::list<std::pair<std::string, std::string> > HeadersType;
class HeadersBuilder;
class Headers {
   public:
    static const std::string eol;
    Headers() {}
    Headers(const HeadersBuilder &);
    std::string toString() const;
    operator std::string() const { return toString(); }
    std::string get(const std::string &key) const;
    std::set<std::string> names() const;
    HeadersBuilder newBuilder() const;

   private:
    HeadersType headers_;
    friend class HeadersBuilder;
};

//------------------------------------------------------------------------------
class HeadersBuilder {
   public:
    HeadersBuilder() {}
    HeadersBuilder(const Headers &);

    // Copy/move constructors/assignments
    HeadersBuilder(HeadersBuilder &&) = default;
    HeadersBuilder(const HeadersBuilder &) = default;
    HeadersBuilder &operator=(HeadersBuilder &&) = default;
    HeadersBuilder &operator=(const HeadersBuilder &) = default;

    Headers build() const;
    std::string get(const std::string &key) const;

    HeadersBuilder &set(const std::string &key, const std::string &value);
    HeadersBuilder &clear();

    static HeadersBuilder parse(const std::string &);

   private:
    HeadersType headers_;
    friend class Headers;
};

//------------------------------------------------------------------------------

#endif
