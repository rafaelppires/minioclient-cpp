#ifndef _AWS_SIGNER_H_
#define _AWS_SIGNER_H_

#include <httpclient.h>

class Signer {
public:
    static Request signV4(Request request, const std::string& region,
                          const std::string& accessKey,
                          const std::string& secretKey);
};

#endif
