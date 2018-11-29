#include <aws_signer.h>
#include <digest.h>
#include <serversideencryption.h>
#include <stringutils.h>
//------------------------------------------------------------------------------
using namespace StringUtils;
//------------------------------------------------------------------------------
const std::string Signer::ALGORITHM = "AWS4-HMAC-SHA256";
const std::set<std::string> Signer::IGNORED_HEADERS(
    {"authorization", "content-type", "content-length", "user-agent"});

//------------------------------------------------------------------------------
Signer::Signer(Request request, const std::string &contentSha256, DateTime date,
               const std::string &region, const std::string &accessKey,
               const std::string &secretKey, const std::string &prevSignature)
    : request_(request),
      contentSha256_(contentSha256),
      date_(date),
      region_(region),
      accessKey_(accessKey),
      secretKey_(secretKey),
      prevSignature_(prevSignature) {}

//------------------------------------------------------------------------------
Request Signer::signV4(Request request, const std::string &region,
                       const std::string &accessKey,
                       const std::string &secretKey) {
    std::string contentSha256 = request.header("x-amz-content-sha256");
    DateTime date = DateTime::parseDateTime(request.header("x-amz-date"));
    Signer signer(request, contentSha256, date, region, accessKey, secretKey,
                  "");
    signer.setScope()
        .setCanonicalRequest()
        .setStringToSign()
        .setSigningKey()
        .setSignature()
        .setAuthorization();
    return RequestBuilder(request)
        .header("Authorization", signer.authorization_)
        .build();
}

//------------------------------------------------------------------------------
Signer &Signer::setScope() {
    scope_ = date_.toString(DateFormat::SIGNER_DATE_FORMAT) + "/" + region_ +
             "/s3/aws4_request";
    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setCanonicalHeaders() {
    const Headers &headers = request_.headers();
    for (const std::string &name : headers.names()) {
        std::string signedHeader = tolower(name);
        if (IGNORED_HEADERS.find(signedHeader) == IGNORED_HEADERS.end()) {
            canonicalHeaders_[signedHeader] = headers.get(name);
        }
    }

    for (const auto &kv : canonicalHeaders_) {
        signedHeaders_ += kv.first + ';';
    }

    if (!signedHeaders_.empty())
        signedHeaders_.erase(signedHeaders_.size() - 1, 1);

    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setCanonicalQueryString() {
    std::map<std::string, std::string> signedQueryParams;

    std::string encodedQuery = url_.encodedQuery();
    if (encodedQuery == "") {
        canonicalQueryString_ = "";
        return *this;
    }

    for (const std::string &queryParam : split(encodedQuery, "&")) {
        auto tokens = split(queryParam, "=");
        if (tokens.size() > 1) {
            signedQueryParams[tokens[0]] = tokens[1];
        } else {
            signedQueryParams[tokens[0]] = "";
        }
    }

    canonicalQueryString_ =
        Joiner::on("&").withKeyValueSeparator("=").join(signedQueryParams);
    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setCanonicalRequest() {
    setCanonicalHeaders();
    url_ = request_.url();
    setCanonicalQueryString();

    // CanonicalRequest =
    //   HTTPRequestMethod + '\n' +
    //   CanonicalURI + '\n' +
    //   CanonicalQueryString + '\n' +
    //   CanonicalHeaders + '\n' +
    //   SignedHeaders + '\n' +
    //   HexEncode(Hash(RequestPayload))
    canonicalRequest_ =
        request_.method() + "\n" + url_.encodedPath() + "\n" +
        canonicalQueryString_ + "\n" +
        Joiner::on("\n").withKeyValueSeparator(":").join(canonicalHeaders_) +
        "\n\n" + signedHeaders_ + "\n" + contentSha256_;

    canonicalRequestHash_ = Digest::sha256_base16(canonicalRequest_);
    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setStringToSign() {
    stringToSign_ = ALGORITHM + "\n" +
                    date_.toString(DateFormat::AMZ_DATE_FORMAT) + "\n" +
                    scope_ + "\n" + canonicalRequestHash_;
    return *this;
}

//------------------------------------------------------------------------------
#if 0
  Signer &setChunkStringToSign() {
    stringToSign_ = "AWS4-HMAC-SHA256-PAYLOAD" + "\n"
      + date_.toString(DateFormat.AMZ_DATE_FORMAT) + "\n"
      + scope_ + "\n"
      + prevSignature_ + "\n"
      + Digest.sha256Hash("") + "\n"
      + contentSha256_;
    return *this;
  }
#endif

//------------------------------------------------------------------------------
Signer &Signer::setSigningKey() {
    std::string aws4SecretKey = "AWS4" + secretKey_;
    auto dateKey = Digest::hmac_sha256(
        aws4SecretKey, date_.toString(DateFormat::SIGNER_DATE_FORMAT));
    auto dateRegionKey = Digest::hmac_sha256(dateKey, region_);
    auto dateRegionServiceKey =
        Digest::hmac_sha256(dateRegionKey, std::string("s3"));
    signingKey_ =
        Digest::hmac_sha256(dateRegionServiceKey, std::string("aws4_request"));
    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setSignature() {
    auto digest = Digest::hmac_sha256(signingKey_, stringToSign_);
    signature_ = Digest::base16_encode(digest);
    return *this;
}

//------------------------------------------------------------------------------
Signer &Signer::setAuthorization() {
    authorization_ = ALGORITHM + " Credential=" + accessKey_ + "/" + scope_ +
                     ", SignedHeaders=" + signedHeaders_ +
                     ", Signature=" + signature_;
    return *this;
}

//------------------------------------------------------------------------------
#if 0
  public static String getChunkSignature(String chunkSha256, DateTime date_, String region_, String secretKey_,
                                         String prevSignature_)
    throws NoSuchAlgorithmException, InvalidKeyException {
    Signer signer = new Signer(null, chunkSha256, date_, region_, null, secretKey_, prevSignature_);
    signer.setScope();
    signer.setChunkStringToSign();
    signer.setSigningKey();
    signer.setSignature();

    return signer.signature_;
  }

//------------------------------------------------------------------------------
  public static String getChunkSeedSignature(Request request_, String region_, String secretKey_)
    throws NoSuchAlgorithmException, InvalidKeyException {
    String contentSha256_ = request_.header("x-amz-content-sha256");
    DateTime date_ = DateFormat.AMZ_DATE_FORMAT.parseDateTime(request_.header("x-amz-date_"));

    Signer signer = new Signer(request_, contentSha256_, date_, region_, null, secretKey_, null);
    signer.setScope();
    signer.setCanonicalRequest();
    signer.setStringToSign();
    signer.setSigningKey();
    signer.setSignature();
    
    return signer.signature_;
  }


//------------------------------------------------------------------------------
  private void setPresignCanonicalRequest(int expires) throws NoSuchAlgorithmException {
    canonicalHeaders_ = new TreeMap<>();
    canonicalHeaders_.put("host", request_.headers().get("Host"));
    signedHeaders_ = "host";

    HttpUrl.Builder urlBuilder = request_.url_().newBuilder();
    // order of queryparam addition is important ie has to be sorted.
    urlBuilder.addEncodedQueryParameter(S3Escaper.encode("X-Amz-Algorithm"),
                                        S3Escaper.encode("AWS4-HMAC-SHA256"));
    urlBuilder.addEncodedQueryParameter(S3Escaper.encode("X-Amz-Credential"),
                                        S3Escaper.encode(accessKey_ + "/" + scope_));
    urlBuilder.addEncodedQueryParameter(S3Escaper.encode("X-Amz-Date"),
                                        S3Escaper.encode(date_.toString(DateFormat.AMZ_DATE_FORMAT)));
    urlBuilder.addEncodedQueryParameter(S3Escaper.encode("X-Amz-Expires"),
                                        S3Escaper.encode(Integer.toString(expires)));
    urlBuilder.addEncodedQueryParameter(S3Escaper.encode("X-Amz-SignedHeaders"),
                                        S3Escaper.encode(signedHeaders_));
    url_ = urlBuilder.build();

    setCanonicalQueryString();

    canonicalRequest_ = request_.method() + "\n"
      + url_.encodedPath() + "\n"
      + canonicalQueryString_ + "\n"
      + Joiner.on("\n").withKeyValueSeparator(":").join(canonicalHeaders_) + "\n\n"
      + signedHeaders_ + "\n"
      + contentSha256_;

    canonicalRequestHash_ = Digest.sha256Hash(canonicalRequest_);
  }

//------------------------------------------------------------------------------
  public static HttpUrl presignV4(Request request_, String region_, String accessKey_, String secretKey_, int expires)
    throws NoSuchAlgorithmException, InvalidKeyException {
    String contentSha256_ = "UNSIGNED-PAYLOAD";
    DateTime date_ = DateFormat.AMZ_DATE_FORMAT.parseDateTime(request_.header("x-amz-date_"));

    Signer signer = new Signer(request_, contentSha256_, date_, region_, accessKey_, secretKey_, null);
    signer.setScope();
    signer.setPresignCanonicalRequest(expires);
    signer.setStringToSign();
    signer.setSigningKey();
    signer.setSignature();

    return signer.url_.newBuilder()
        .addEncodedQueryParameter(S3Escaper.encode("X-Amz-Signature"), S3Escaper.encode(signer.signature_))
        .build();
  }

//------------------------------------------------------------------------------
  public static String credential(String accessKey_, DateTime date_, String region_) {
    return accessKey_ + "/" + date_.toString(DateFormat.SIGNER_DATE_FORMAT) + "/" + region_ + "/s3/aws4_request";
  }

//------------------------------------------------------------------------------
  public static String postPresignV4(String stringToSign_, String secretKey_, DateTime date_, String region_)
    throws NoSuchAlgorithmException, InvalidKeyException {
    Signer signer = new Signer(null, null, date_, region_, null, secretKey_, null);
    signer.stringToSign_ = stringToSign_;
    signer.setSigningKey();
    signer.setSignature();

    return signer.signature_;
  }

//------------------------------------------------------------------------------
  public static byte[] sumHmac(byte[] key, byte[] data)
    throws NoSuchAlgorithmException, InvalidKeyException {
    Mac mac = Mac.getInstance("HmacSHA256");

    mac.init(new SecretKeySpec(key, "HmacSHA256"));
    mac.update(data);

    return mac.doFinal();
  }
#endif
//------------------------------------------------------------------------------
