#ifndef _MINIO_CLIENT_H_
#define _MINIO_CLIENT_H_
#include <dataobject.h>
#include <httpclient.h>
#include <httpresponse.h>
#include <logprinter.h>
#include <minioexceptions.h>
#include <serversideencryption.h>
#include <map>
#include <string>

class MinioClient {
   public:
    static const std::string US_EAST_1;
    static const std::string UPLOAD_ID;
    static const std::string END_HTTP;
    static const std::string NULL_STRING;
    static const std::string S3_AMAZONAWS_COM;
    static const std::string DEFAULT_USER_AGENT;
    static const long MAX_OBJECT_SIZE;
    static const int MIN_MULTIPART_SIZE;

    MinioClient(const std::string &endpoint, const std::string &accessKey,
                const std::string &secretKey)
        : MinioClient(endpoint, 0, accessKey, secretKey) {}
    MinioClient(const std::string &endpoint, int port,
                const std::string &accessKey, const std::string &secretKey)
        : MinioClient(endpoint, port, accessKey, secretKey,
                      endpoint.find("https://") == 0) {}
    MinioClient(const std::string &endpoint, int port,
                const std::string &accessKey, const std::string &secretKey,
                bool secure)
        : MinioClient(endpoint, port, accessKey, secretKey, "", secure) {}
    MinioClient(const std::string &endpoint, int port,
                const std::string &accessKey, const std::string &secretKey,
                const std::string &region, bool secure)
        : MinioClient(endpoint, port, accessKey, secretKey, region, secure,
                      nullptr) {}
    MinioClient(const std::string &endpoint, int port,
                const std::string &accessKey, const std::string &secretKey,
                const std::string &region, bool secure, HttpClient *httpClient);

    void traceOn(std::basic_ostream<char> &stream);
    void makeBucket(const std::string &bucketName,
                    const std::string &region = "");
    bool bucketExists(const std::string &bucketName);

    void putObject(const std::string &bucketName, const std::string &objectName,
                   const std::string &fileName, const std::string &contentType);
    void putObject(const std::string &bucketName, const std::string &objectName,
                   const std::string &fileName);
    /*void putObject(const std::string &bucketName, const std::string
    &objectName, std::ifstream &stream, long size, const std::string
    &contentType); void putObject(const std::string &bucketName, const
    std::string &objectName, std::ifstream &stream, long size, const KeyValueMap
    &headerMap); void putObject(const std::string &bucketName, const std::string
    &objectName, std::ifstream &stream, long size, ServerSideEncryption sse);
    void putObject(const std::string &bucketName, const std::string &objectName,
                   std::ifstream &stream, const std::string &contentType);
    */
   private:
    std::string putObject(const std::string &bucketName,
                          const std::string &objectName, int length,
                          const ByteArray &data, const std::string &uploadId,
                          int partNumber, const KeyValueMap &headerMap);
    void putObject(const std::string &bucketName, const std::string &objectName,
                   long size, const ByteArray &data, KeyValueMap headerMap,
                   ServerSideEncryption *sse);

    HttpResponse executePut(const std::string &bucketName,
                            const std::string &objectName,
                            const KeyValueMap &headerMap,
                            const KeyValueMap &queryParamMap,
                            const std::string &region, const ByteArray &data,
                            int length);
    HttpResponse executePut(const std::string &bucketName,
                            const std::string &objectName,
                            const KeyValueMap &headerMap,
                            const KeyValueMap &queryParamMap,
                            const ByteArray &data, int length);

    HttpResponse execute(Method method, const std::string &region,
                         const std::string &bucketName,
                         const std::string &objectName,
                         const KeyValueMap &headerMap,
                         const KeyValueMap &queryParamMap,
                         const ByteArray &body);

    HttpResponse executeHead(const std::string &bucketName,
                             const std::string &objectName);
    HttpResponse executeHead(const std::string &bucketName,
                             const std::string &objectName,
                             const KeyValueMap &headerMap);

    Request createRequest(Method method, const std::string &bucketName,
                          const std::string &objectName,
                          const std::string &region,
                          const KeyValueMap &headerMap,
                          const KeyValueMap &queryParamMap,
                          const std::string &contentType,
                          const ByteArray &body);
    void checkBucketName(const std::string &name);

    ErrorResponseException handleError(const std::string &objectName,
                                       const std::string &bucketName,
                                       const Response &response);
    bool shouldOmitPortInHostHeader(const HttpUrl &url) {
        return (url.isHttps() && url.port() == 443) ||
               (!url.isHttps() && url.port() == 80);
    }

    std::string getRegion(const std::string &);
    std::string region_, accessKey_, secretKey_, userAgent_;
    LogPrinter *traceStream_;
    HttpClient httpClient_;
    HttpUrl baseUrl_;
};

#endif
