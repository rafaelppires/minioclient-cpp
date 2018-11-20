#ifndef _SERVER_SIDE_ENCRYPTION_H_
#define _SERVER_SIDE_ENCRYPTION_H_

#include <map>
#include <string>

class ServerSideEncryption {
   public:
    void marshall(std::map<std::string, std::string> &headers) const {}
};

#endif
