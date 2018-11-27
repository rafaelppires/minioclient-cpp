#ifndef _URL_BUILDER_H_
#define _URL_BUILDER_H_

#include <string>
#include <vector>

//------------------------------------------------------------------------------
class Escaper {
   public:
    static std::string encode(std::string str);
};

//------------------------------------------------------------------------------
class Scheme {
   public:
    enum Schemes { HTTP, HTTPS };
    Scheme(Schemes s) : s_(s) {}
    std::string toString();

   private:
    Schemes s_;
};

//------------------------------------------------------------------------------
class UrlBuilder;
class HttpUrl {
   public:
    HttpUrl() {}
    HttpUrl(const UrlBuilder &);
    std::string encodedPath() const;
    std::string encodedQuery() const;
    std::string baseUrl() const;
    std::string toString() const { return url_; }
    bool isHttps() const { return scheme_ == "https"; }
    int port() const { return port_; }
    std::string host() const { return host_; }
    UrlBuilder newBuilder();

    static HttpUrl parse(const std::string &endpoint);

   private:
    std::string scheme_, host_, url_;
    int port_;
};

//------------------------------------------------------------------------------
class UrlBuilder {
   public:
    UrlBuilder();

    UrlBuilder &host(const std::string &host);
    UrlBuilder &port(int p);
    UrlBuilder &scheme(const std::string &s);

    void addEncodedPathSegment(const std::string &ps);
    void addPathSegment(const std::string &ps);
    void addEncodedQueryParameter(const std::string &key,
                                  const std::string &value);
    int effectivePort() const;
    void url(const HttpUrl &);
    HttpUrl build();
    std::string toString() const;

    static UrlBuilder parse(const std::string &input);
    static int defaultPort(const std::string &scheme);

   private:
    std::string scheme_, host_;
    int port_;
    std::vector<std::string> path_segments_;
    std::vector<std::pair<std::string, std::string> > encoded_query_params_;
    friend class HttpUrl;
};

//------------------------------------------------------------------------------
#endif
