#ifndef _AWS_SIGNER_H_
#define _AWS_SIGNER_H_

#include <httpclient.h>
#include <httpresponse.h>
#include <map>
#include <set>
#include <datetime.h>

//------------------------------------------------------------------------------
class Signer {
   public:
    Signer(Request request, const std::string &contentSha256, DateTime date,
           const std::string &region, const std::string &accessKey,
           const std::string &secretKey, const std::string &prevSignature);

    static Request signV4(Request request, const std::string &region,
                          const std::string &accessKey,
                          const std::string &secretKey);

   private:
    static const std::set<std::string> IGNORED_HEADERS;
    static const std::string ALGORITHM;

    Request request_;
    std::string contentSha256_;
    DateTime date_;
    std::string region_;
    std::string accessKey_;
    std::string secretKey_;
    std::string prevSignature_;

    std::string scope_;
    std::map<std::string, std::string> canonicalHeaders_;
    std::string signedHeaders_;
    HttpUrl url_;
    std::string canonicalQueryString_;
    std::string canonicalRequest_;
    std::string canonicalRequestHash_;
    std::string stringToSign_;
    std::vector<char> signingKey_;
    std::string signature_;
    std::string authorization_;

    void setScope();
    void setCanonicalHeaders();
    void setCanonicalQueryString();
    void setCanonicalRequest();
    void setStringToSign();
    //void setChunkStringToSign();
    void setSigningKey();
    void setSignature();
    void setAuthorization();
};

#endif
