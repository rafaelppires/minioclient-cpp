#include <aws_signer.h>

Request Signer::signV4(Request request, String region, String accessKey, String secretKey)
    throws NoSuchAlgorithmException, InvalidKeyException {
    String contentSha256 = request.header("x-amz-content-sha256");
    DateTime date = DateFormat.AMZ_DATE_FORMAT.parseDateTime(request.header("x-amz-date"));

    Signer signer = new Signer(request, contentSha256, date, region, accessKey, secretKey, null);
    signer.setScope();
    signer.setCanonicalRequest();
    signer.setStringToSign();
    signer.setSigningKey();
    signer.setSignature();
    signer.setAuthorization();

    return request.newBuilder().header("Authorization", signer.authorization).build();
  }

