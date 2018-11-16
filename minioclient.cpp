#include <aws_signer.h>
#include <files.h>
#include <logprinter.h>
#include <minioclient.h>
#include <minioexceptions.h>
#include <stringutils.h>
#include <map>
#include <string>
using namespace StringUtils;

const std::string MinioClient::US_EAST_1 = "us-east-1";
const std::string MinioClient::UPLOAD_ID = "uploadId";
const std::string MinioClient::NULL_STRING = "(null)";
const std::string MinioClient::S3_AMAZONAWS_COM = "s3.amazonaws.com";
const std::string MinioClient::END_HTTP = "----------END-HTTP----------";
const std::string MinioClient::DEFAULT_USER_AGENT = "A-SKY 0.0001";
const long MinioClient::MAX_OBJECT_SIZE = 5L * 1024 * 1024 * 1024 * 1024;
const int MinioClient::MIN_MULTIPART_SIZE = 5 * 1024 * 1024;
//------------------------------------------------------------------------------
MinioClient::MinioClient(const std::string &endpoint, int port,
                         const std::string &accessKey,
                         const std::string &secretKey,
                         const std::string &region, bool secure,
                         HttpClient *httpClient) : traceStream_(nullptr) {
    if (endpoint.empty()) {
        throw InvalidEndpointException(NULL_STRING, "null endpoint");
    }

    if (port < 0 || port > 65535) {
        throw InvalidPortException(port, "port must be in range of 1 to 65535");
    }

#if 0
    if (httpClient != nullptr) {
      httpClient_ = httpClient;
    } else {
      this.httpClient = new OkHttpClient();
      this.httpClient = this.httpClient.newBuilder()
        .connectTimeout(DEFAULT_CONNECTION_TIMEOUT, TimeUnit.SECONDS)
        .writeTimeout(DEFAULT_CONNECTION_TIMEOUT, TimeUnit.SECONDS)
        .readTimeout(DEFAULT_CONNECTION_TIMEOUT, TimeUnit.SECONDS)
        .build();
    }
#endif

    HttpUrl url = HttpUrl::parse(endpoint);
    if (url.encodedPath() != "/") {
        throw InvalidEndpointException(endpoint, "no path allowed in endpoint");
    }

    UrlBuilder urlBuilder = url.newBuilder();
    Scheme scheme = Scheme::HTTP;
    if (secure) {
        scheme = Scheme::HTTPS;
    }

    urlBuilder.scheme(scheme.toString());
    urlBuilder.port(port);
    baseUrl_ = urlBuilder.build();

    accessKey_ = accessKey;
    secretKey_ = secretKey;
    region_ = region;
}
//------------------------------------------------------------------------------
void MinioClient::makeBucket(const std::string &bucketName,
                             const std::string &region) {
    // If region param is not provided, set it with the one provided by
    // constructor
    const std::string &reg = region.empty() ? region_ : region;

    // If constructor already sets a region, check if it is equal to region
    // param if provided
    if (!reg.empty() && reg != region_) {
        throw RegionConflictException(
            "passed region conflicts with the one previously specified");
    }
    std::string configString;
    if (reg.empty() || reg == US_EAST_1) {
        // for 'us-east-1', location constraint is not required.  for more
        // info
        // http://docs.aws.amazon.com/general/latest/gr/rande.html#s3_region
        configString = "";
    } else {
        configString =
            "<?xml version=\"1.0\"?><CreateBucketConfiguration "
            "xmlns=\"http://s3.amazonaws.com/doc/2006-03-01/"
            "\"><LocationConstraint>" +
            reg + "</LocationConstraint></CreateBucketConfiguration>";
    }

    KeyValueMap empty;
    ByteArray data(configString.begin(), configString.end());
    HttpResponse response =
        executePut(bucketName, "", empty, empty, US_EAST_1, data, 0);
    // response.body().close();
}

//------------------------------------------------------------------------------
bool MinioClient::bucketExists(const std::string &bucketName) {
    try {
        executeHead(bucketName, "");
        return true;
    } catch (const ErrorResponseException &e) {
        if (e.errorResponse().errorCode() != ErrorCode::NO_SUCH_BUCKET) {
            throw e;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
HttpResponse MinioClient::executeHead(const std::string &bucketName,
                                      const std::string &objectName) {
    KeyValueMap empty;
    HttpResponse response =
        execute(Method::HEAD, getRegion(bucketName), bucketName, objectName,
                empty, empty, ByteArray());
    // response.body().close();
    return response;
}

//------------------------------------------------------------------------------
HttpResponse MinioClient::executeHead(const std::string &bucketName,
                                      const std::string &objectName,
                                      const KeyValueMap &headerMap) {
    KeyValueMap empty;
    HttpResponse response =
        execute(Method::HEAD, getRegion(bucketName), bucketName, objectName,
                headerMap, empty, ByteArray());
    // response.body().close();
    return response;
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            const std::string &fileName,
                            const std::string &contentType) {
    if (fileName.empty()) {
        throw std::invalid_argument("empty file name is not allowed");
    }

    /*
    std::string filePath = Paths::get(fileName);
    if (!Files::isRegularFile(filePath)) {
        throw std::invalid_argument("'" + fileName + "': not a regular
    file");
    }
    */

    std::string default_contenttype = "text/plain";
    const std::string &contenttype =
        contentType.empty() ? default_contenttype : contentType;

    /* if (contentType == "") {
        contentType = Files::probeContentType(filePath);
    }*/

    long size = Files::size(fileName);

    ByteArray data = Files::readAll(fileName);

    // Set the contentType
    KeyValueMap headerMap;
    headerMap["Content-Type"] = contenttype;

    putObject(bucketName, objectName, size, data, headerMap, nullptr);
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            const std::string &fileName) {
    putObject(bucketName, objectName, fileName, "");
}

#if 0
//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            std::ifstream &stream, long size,
                            const std::string &contentType) {
    // Set the contentType
    const KeyValueMap headerMap;
    headerMap["Content-Type"] = contentType;

    putObject(bucketName, objectName, size, new Bufferedstd::ifstream &(stream),
              headerMap, null);
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            std::ifstream &stream, long size,
                            const KeyValueMap &headerMap) {
    if (headerMap == null) {
        headerMap = new HashMap<>();
    }

    putObject(bucketName, objectName, size, new Bufferedstd::ifstream &(stream),
              headerMap, null);
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            std::ifstream &stream, long size,
                            ServerSideEncryption sse) {
    if ((sse.getType() == (ServerSideEncryption.Type.SSE_C)) &&
        (!this.baseUrl.isHttps())) {
        throw std::invalid_argument(
            "SSE_C operations must be performed over a secure connection.");
    } else if ((sse.getType() == (ServerSideEncryption.Type.SSE_KMS)) &&
               (!this.baseUrl.isHttps())) {
        throw std::invalid_argument(
            "SSE_KMS operations must be performed over a secure connection.");
    }
    const KeyValueMap &headerMap = new HashMap<>();
    putObject(bucketName, objectName, size, new Bufferedstd::ifstream &(stream),
              headerMap, sse);
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName,
                            std::ifstream &stream,
                            const std::string &contentType) {
    // Set the contentType
    const KeyValueMap &headerMap = new HashMap<>();
    headerMap.put("Content-Type", contentType);

    putObject(bucketName, objectName, null, new Bufferedstd::ifstream &(stream),
              headerMap, null);
}
#endif
//------------------------------------------------------------------------------
std::string MinioClient::putObject(const std::string &bucketName,
                                   const std::string &objectName, int length,
                                   const ByteArray &data,
                                   const std::string &uploadId, int partNumber,
                                   const KeyValueMap &headerMap) {
    HttpResponse response;

    KeyValueMap queryParamMap;
    if (partNumber > 0 && !uploadId.empty()) {
        queryParamMap["partNumber"] = std::to_string(partNumber);
        queryParamMap[UPLOAD_ID] = uploadId;
    }

    response = executePut(bucketName, objectName, headerMap, queryParamMap,
                          data, length);

    // response.body().close();
    return response.header().etag();
}

//------------------------------------------------------------------------------
void MinioClient::putObject(const std::string &bucketName,
                            const std::string &objectName, long size,
                            const ByteArray &data, KeyValueMap headerMap,
                            ServerSideEncryption *sse) {
    bool unknownSize = false;

    // Add content type if not already set
    if (headerMap.find("Content-Type") == headerMap.end()) {
        headerMap["Content-Type"] = "application/octet-stream";
    }

    if (size < 0) {
        unknownSize = true;
        size = MAX_OBJECT_SIZE;
    }

    if (size <= MIN_MULTIPART_SIZE) {
        // Single put object.
        if (sse != nullptr) {
            sse->marshall(headerMap);
        }
        putObject(bucketName, objectName, size, data, "", 0, headerMap);
        return;
    }
#if 0
    /* Multipart upload */
    int[] rv = calculateMultipartSize(size);
    int partSize = rv[0];
    int partCount = rv[1];
    int lastPartSize = rv[2];
    Part[] totalParts = new Part[partCount];

    // check whether there is incomplete multipart upload or not
    const std::string &uploadId =
        getLatestIncompleteUploadId(bucketName, objectName);
    Iterator<Result<Part>> existingParts = null;
    Part part = null;
    bool isResumeMultipart = false;
    if (uploadId != null) {
        isResumeMultipart = true;
        // resume previous multipart upload
        existingParts =
            listObjectParts(bucketName, objectName, uploadId).iterator();
        if (existingParts.hasNext()) {
            part = existingParts.next().get();
        }
    } else {
        // initiate new multipart upload ie no previous multipart found or no
        // previous valid parts for multipart found
        if (sse != null) {
            sse.marshal(headerMap);
        }
        uploadId = initMultipartUpload(bucketName, objectName, headerMap);
    }

    int expectedReadSize = partSize;
    for (int partNumber = 1; partNumber <= partCount; partNumber++) {
        if (partNumber == partCount) {
            expectedReadSize = lastPartSize;
        }

        // For unknown sized stream, check available size.
        int availableSize = 0;
        if (unknownSize) {
            // Check whether data is available one byte more than
            // expectedReadSize.
            availableSize = getAvailableSize(data, expectedReadSize + 1);
            // If availableSize is less or equal to expectedReadSize, then we
            // reached last part.
            if (availableSize <= expectedReadSize) {
                // If it is first part, do single put object.
                if (partNumber == 1) {
                    putObject(bucketName, objectName, availableSize, data, null,
                              0, headerMap);
                    // if its not resuming previous multipart, remove newly
                    // created multipart upload.
                    if (!isResumeMultipart) {
                        abortMultipartUpload(bucketName, objectName, uploadId);
                    }
                    return;
                }

                expectedReadSize = availableSize;
                partCount = partNumber;
            }
        }

        if (part != null && partNumber == part.partNumber() &&
            expectedReadSize == part.partSize()) {
            const std::string &md5Hash = Digest.md5Hash(data, expectedReadSize);
            if (md5Hash.equals(part.etag())) {
                // this part is already uploaded
                totalParts[partNumber - 1] =
                    new Part(part.partNumber(), part.etag());
                skipStream(data, expectedReadSize);

                part = getPart(existingParts);

                continue;
            }
        }

        // In multi-part uploads, Set encryption headers in the case of SSE-C.
        KeyValueMap encryptionHeaders;
        if (sse != null && sse.getType() == ServerSideEncryption.Type.SSE_C) {
            sse.marshal(encryptionHeaders);
        }

        const std::string &etag =
            putObject(bucketName, objectName, expectedReadSize, data, uploadId,
                      partNumber, encryptionHeaders);
        totalParts[partNumber - 1] = new Part(partNumber, etag);
    }

    completeMultipart(bucketName, objectName, uploadId, totalParts);
#endif
}

//------------------------------------------------------------------------------
HttpResponse MinioClient::executePut(const std::string &bucketName,
                                     const std::string &objectName,
                                     const KeyValueMap &headerMap,
                                     const KeyValueMap &queryParamMap,
                                     const std::string &region,
                                     const ByteArray &data, int length) {
    HttpResponse response = execute(Method::PUT, region, bucketName, objectName,
                                    headerMap, queryParamMap, data);
    return response;
}

//------------------------------------------------------------------------------
HttpResponse MinioClient::executePut(const std::string &bucketName,
                                     const std::string &objectName,
                                     const KeyValueMap &headerMap,
                                     const KeyValueMap &queryParamMap,
                                     const ByteArray &data, int length) {
    return executePut(bucketName, objectName, headerMap, queryParamMap,
                      getRegion(bucketName), data, length);
}

//------------------------------------------------------------------------------
HttpResponse MinioClient::execute(Method method, const std::string &region,
                                  const std::string &bucketName,
                                  const std::string &objectName,
                                  const KeyValueMap &headerMap,
                                  const KeyValueMap &queryParamMap,
                                  const ByteArray &body) {
    std::string contentType;
    KeyValueMap::const_iterator found;
    if (headerMap.find("Content-Type") != headerMap.end()) {
        contentType = found->second;
    }

    Request request =
        createRequest(method, bucketName, objectName, region, headerMap,
                      queryParamMap, contentType, body);

    if (!accessKey_.empty() && !secretKey_.empty()) {
        request = Signer::signV4(request, region, accessKey_, secretKey_);
    }

    if (traceStream_ != nullptr) {
        traceStream_->println("---------START-HTTP---------");
        std::string encodedPath = request.url().encodedPath();
        std::string encodedQuery = request.url().encodedQuery();
        if (!encodedQuery.empty()) {
            encodedPath += "?" + encodedQuery;
        }
        traceStream_->println(request.method() + " " + encodedPath +
                              " HTTP/1.1");
        std::string headers = request.headers().toString();
#if 0
        // WARNING WARNING WARNING
                .replaceAll("Signature=([0-9a-f]+)", "Signature=*REDACTED*")
                .replaceAll("Credential=([^/]+)", "Credential=*REDACTED*");
#endif
        traceStream_->println(headers);
    }

    Response response = httpClient_.newCall(request).execute();
    if (response.empty()) {
        if (traceStream_ != nullptr) {
            traceStream_->println("<NO RESPONSE>");
            traceStream_->println(END_HTTP);
        }
        throw NoResponseException();
    }

    if (traceStream_ != nullptr) {
        traceStream_->println(response.protocol() + " " +
                              std::to_string(response.code()));
        traceStream_->println(response.headers());
    }

    // ResponseHeader header;
    // HeaderParser.set(response.headers(), header);

    if (response.isSuccessful()) {
        if (traceStream_ != nullptr) {
            traceStream_->println(END_HTTP);
        }
        return HttpResponse(response);
    }

#if 0
    ErrorResponse errorResponse = nullptr;

    // HEAD returns no body, and fails on parseXml
    if (!method.equals(Method::HEAD)) {
        Scanner scanner = new Scanner(response.body().charStream());
        try {
            scanner.useDelimiter("\\A");
            std::string errorXml = "";

            // read entire body stream to string.
            if (scanner.hasNext()) {
                errorXml = scanner.next();
            }

            errorResponse = new ErrorResponse(new StringReader(errorXml));
            if (traceStream_ != nullptr) {
                traceStream_->println(errorXml);
            }

        } finally {
            response.body().close();
            scanner.close();
        }
    }

    if (traceStream_ != nullptr) {
        traceStream_->println(END_HTTP);
    }
    if (errorResponse == null) {
        ErrorCode ec;
        switch (response.code()) {
            case 307:
                ec = ErrorCode.REDIRECT;
                break;
            case 400:
                ec = ErrorCode.INVALID_URI;
                break;
            case 404:
                if (objectName != null) {
                    ec = ErrorCode.NO_SUCH_KEY;
                } else if (bucketName != null) {
                    ec = ErrorCode.NO_SUCH_BUCKET;
                } else {
                    ec = ErrorCode.RESOURCE_NOT_FOUND;
                }
                break;
            case 501:
            case 405:
                ec = ErrorCode.METHOD_NOT_ALLOWED;
                break;
            case 409:
                if (bucketName != null) {
                    ec = ErrorCode.NO_SUCH_BUCKET;
                } else {
                    ec = ErrorCode.RESOURCE_CONFLICT;
                }
                break;
            case 403:
                ec = ErrorCode.ACCESS_DENIED;
                break;
            default:
                throw InternalException(
                    "unhandled HTTP code " + response.code() +
                    ".  Please report this issue at " +
                    "https://github.com/minio/minio-java/issues");
        }

        errorResponse = new ErrorResponse(
            ec, bucketName, objectName, request.url().encodedPath(),
            header.xamzRequestId(), header.xamzId2());
    }

    // invalidate region cache if needed
    if (errorResponse.errorCode() == ErrorCode.NO_SUCH_BUCKET) {
        BucketRegionCache.INSTANCE.remove(bucketName);
        // TODO: handle for other cases as well
        // observation: on HEAD of a bucket with wrong region gives 400 without
        // body
    }

#endif
    // throw ErrorResponseException(errorResponse, response);
    throw ErrorResponseException("errorResponse");
}

//------------------------------------------------------------------------------
Request MinioClient::createRequest(Method method, const std::string &bucketName,
                                   const std::string &objectName,
                                   const std::string &region,
                                   const KeyValueMap &headerMap,
                                   const KeyValueMap &queryParamMap,
                                   const std::string &contentType,
                                   const ByteArray &body) {
    if (bucketName.empty() && !objectName.empty()) {
        throw InvalidBucketNameException(
            NULL_STRING, "null bucket name for object '" + objectName + "'");
    }

    UrlBuilder urlBuilder = baseUrl_.newBuilder();
    if (!bucketName.empty()) {
        checkBucketName(bucketName);

#if 0
        std::string host = baseUrl_.host();
        if (host == S3_AMAZONAWS_COM) {
            // special case: handle s3.amazonaws.com separately
            if (!region.empty()) {
                host = AwsS3Endpoints.INSTANCE.endpoint(region);
            }

            bool usePathStyle = false;
            if (method == Method.PUT && objectName.empty() &&
                queryParamMap.empty()) {
                // use path style for make bucket to workaround
                // "AuthorizationHeaderMalformed" error from s3.amazonaws.com
                usePathStyle = true;
            } else if (queryParamMap.find("location") != queryParamMap.end()) {
                // use path style for location query
                usePathStyle = true;
            } else if (bucketName.find(".") != std::string::npos  && baseUrl_.isHttps()) {
                // use path style where '.' in bucketName causes SSL certificate
                // validation error
                usePathStyle = true;
            }

            if (usePathStyle) {
                urlBuilder.host(host);
                urlBuilder.addEncodedPathSegment(S3Escaper::encode(bucketName));
            } else {
                urlBuilder.host(bucketName + "." + host);
            }
        } else {
#endif
        urlBuilder.addEncodedPathSegment(Escaper::encode(bucketName));
        //        }
    }

    if (!objectName.empty()) {
        // Limitation: OkHttp does not allow to add '.' and '..' as path
        // segment.
        for (const std::string &pathSegment : split(objectName, "/")) {
            urlBuilder.addEncodedPathSegment(Escaper::encode(pathSegment));
        }
        if (!objectName.empty() && objectName[objectName.size() - 1] == '/') {
            // Fix issue #648: preserve trailing '/' to work with proxies
            // like nginx.
            urlBuilder.addPathSegment("");
        }
    }

    if (!queryParamMap.empty()) {
        for (auto entry : queryParamMap) {
            urlBuilder.addEncodedQueryParameter(Escaper::encode(entry.first),
                                                Escaper::encode(entry.second));
        }
    }

    HttpUrl url = urlBuilder.build();

    RequestBuilder requestBuilder;
    requestBuilder.url(url);
    for (auto entry : headerMap) {
        requestBuilder.header(entry.first, entry.second);
    }

    std::string sha256Hash;
    std::string md5Hash;
    bool chunkedUpload = false;
    if (!accessKey_.empty() && !secretKey_.empty()) {
        // Handle putobject specially to use chunked upload.
        if (method == Method::PUT && !objectName.empty() && !body.empty()) {
            sha256Hash = "STREAMING-AWS4-HMAC-SHA256-PAYLOAD";
            requestBuilder.header("Content-Encoding", "aws-chunked");
            requestBuilder.header("x-amz-decoded-content-length",
                                  std::to_string(body.size()));
            chunkedUpload = true;
        } else if (url.isHttps()) {
            // Fix issue #415: No need to compute sha256 if endpoint scheme
            // is HTTPS.
            sha256Hash = "UNSIGNED-PAYLOAD";
            if (!body.empty()) {
                md5Hash = Digest::md5_base64(body);
            }
        } else {
            ByteArray data = body;
            int len = body.size();

            if (method == Method::POST &&
                queryParamMap.find("delete") != queryParamMap.end()) {
                // Fix issue #579: Treat 'Delete Multiple Objects' specially
                // which requires MD5 hash.
                sha256Hash = Digest::sha256_base64(data);
                md5Hash = Digest::md5_base64(data);
                ;
            } else {
                // Fix issue #567: Compute SHA256 hash only.
                sha256Hash = Digest::sha256_base64(data);
            }
        }
    } else {
        // Fix issue #567: Compute MD5 hash only for anonymous access.
        if (!body.empty()) {
            md5Hash = Digest::md5_base64(body);
        }
    }

    if (!md5Hash.empty()) {
        requestBuilder.header("Content-MD5", md5Hash);
    }
    if (shouldOmitPortInHostHeader(url)) {
        requestBuilder.header("Host", url.host());
    } else {
        requestBuilder.header("Host",
                              url.host() + ":" + std::to_string(url.port()));
    }
    requestBuilder.header("User-Agent", userAgent_);
    if (!sha256Hash.empty()) {
        requestBuilder.header("x-amz-content-sha256", sha256Hash);
    }
    DateTime date;
    requestBuilder.header("x-amz-date",
                          date.toString(DateFormat::AMZ_DATE_FORMAT));
// date.toString(DateFormat.AMZ_DATE_FORMAT));

#if 0
    if (chunkedUpload) {
        // Add empty request body for calculating seed signature.
        // The actual request body is properly set below.
        requestBuilder.method(method.toString(), RequestBody.create(nullptr, ""));
        Request request = requestBuilder.build();
        std::string seedSignature =
            Signer.getChunkSeedSignature(request, region, secretKey);
        requestBuilder = request.newBuilder();

        ChunkedInputStream cis =
            new ChunkedInputStream((InputStream)body, length, date, region,
                                   this.secretKey, seedSignature);
        body = cis;
    }
#endif

    RequestBody requestBody;
    if (!body.empty()) {
        requestBody = RequestBody::create(contentType, body);
    }

    requestBuilder.method(method.toString(), requestBody);
    return requestBuilder.build();
}
//------------------------------------------------------------------------------
void MinioClient::checkBucketName(const std::string &name) {
    if (name.empty()) {
        throw InvalidBucketNameException(NULL_STRING, "null bucket name");
    }

    // Bucket names cannot be no less than 3 and no more than 63 characters
    // long.
    if (name.size() < 3 || name.size() > 63) {
        std::string msg =
            "bucket name must be at least 3 and no more than 63 characters "
            "long";
        throw InvalidBucketNameException(name, msg);
    }
#if 0
    // Successive periods in bucket names are not allowed.
    if (name.matches("\\.\\.")) {
        String msg =
            "bucket name cannot contain successive periods. For more "
            "information refer " +
            "http://docs.aws.amazon.com/AmazonS3/latest/dev/"
            "BucketRestrictions.html";
        throw InvalidBucketNameException(name, msg);
    }
    // Bucket names should be dns compatible.
    if (!name.matches("^[a-z0-9][a-z0-9\\.\\-]+[a-z0-9]$")) {
        String msg =
            "bucket name does not follow Amazon S3 standards. For more "
            "information refer " +
            "http://docs.aws.amazon.com/AmazonS3/latest/dev/"
            "BucketRestrictions.html";
        throw InvalidBucketNameException(name, msg);
    }
#endif
}

//------------------------------------------------------------------------------
void MinioClient::traceOn(std::basic_ostream<char> &stream) {
    if (traceStream_ != nullptr) delete traceStream_;
    traceStream_ = new LogPrinter(stream);
}

//------------------------------------------------------------------------------
