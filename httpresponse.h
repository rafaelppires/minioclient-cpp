#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <vector>

class DateTime {
public:
    std::string toString() {
        return "10_pra_daqui_a_pouco";
    }
};

class Method {
   public:
    enum Methods { HEAD, PUT, POST };
    Method(Methods m) : m_(m) {}
    operator Methods() { return m_; }
    std::string toString() {
        switch (m_) {
            case HEAD:
                return "HEAD";
            case PUT:
                return "PUT";
            case POST:
                return "POST";
        };
    }

   private:
    Methods m_;
};

class Escaper {
   public:
    static std::string encode(std::string str) {
        return str = replaceAll(str, "!", "%21");
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

    static std::vector<std::string> split(const std::string &str,
                                          const std::string sep) {
        char *cstr = const_cast<char *>(str.c_str());
        char *current;
        std::vector<std::string> arr;
        current = strtok(cstr, sep.c_str());
        while (current != NULL) {
            arr.push_back(current);
            current = strtok(NULL, sep.c_str());
        }
        return arr;
    }

   private:
    static std::string replaceAll(std::string str, const std::string &from,
                                  const std::string &to) {
        size_t pos;
        while ((pos = str.find(from)) != std::string::npos) {
            str.replace(pos, from.size(), to);
            pos += to.size();
        }
        return str;
    }
};

class HttpUrl {
   public:
    std::string encodedPath();
    std::string encodedQuery();
    bool isHttps() const;
    unsigned port() const;
    std::string host() const;

   private:
};

class Headers {
   public:
    std::string toString();
    operator const std::string() { return toString(); }

   private:
};

class UrlBuilder {
   public:
    void host(const std::string &host) { host_ = host; }

    void addEncodedPathSegment(const std::string &ps) {
        path_segments_.push_back(ps);
    }

    void addEncodedQueryParameter(const std::string &key,
                                  const std::string &value) {
        encoded_query_params_.push_back(std::make_pair(key, value));
    }

    HttpUrl build();
    void url(const HttpUrl &);

    void addPathSegment(const std::string &ps);

   private:
    std::string host_;
    std::vector<std::string> path_segments_;
    std::vector<std::pair<std::string, std::string> > encoded_query_params_;
};

class ResponseHeader {
   public:
    ResponseHeader();
    ResponseHeader(const Headers &);
    std::string etag() { return etag_; }

   private:
    std::string etag_;  // ETag
};

class RequestBody {
   public:
    static RequestBody create(const std::string &contentType, const std::vector<char> &body);
};

class Request {
   public:
    HttpUrl url();
    std::string method();
    Headers headers();

   private:
};

class RequestBuilder {
   public:
    void url(const HttpUrl &);
    void method(const std::string &method, const RequestBody &body);
    void header(const std::string &key, const std::string &value) {
        headers_.push_back(std::make_pair(key, value));
    }
    Request build();

   private:
    std::vector<std::pair<std::string, std::string> > headers_;
};

class Response {
   public:
    bool empty() { return true; }
    std::string protocol() { return protocol_; }
    std::string body() { return body_; }
    int code() { return status_code_; }
    bool isSuccessful();
    Headers headers() const;

   private:
    std::string body_, protocol_;
    int status_code_;
};

class Call {
   public:
    Response execute();

   private:
};

class HttpClient {
   public:
    Call newCall(const Request &);

   private:
};

class HttpResponse {
   public:
    HttpResponse() {}
    HttpResponse(const Response &r) : header_(r.headers()) {}
    ResponseHeader header() { return header_; }

   private:
    ResponseHeader header_;
    Response response_;
};

#endif
