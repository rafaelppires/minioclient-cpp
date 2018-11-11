#ifndef _SERVER_SIDE_ENCRYPTION_H_
#define _SERVER_SIDE_ENCRYPTION_H_

#include <map>
#include <string>

class Digest{
    public:
    typedef std::vector<char> ByteArray;
    
    static std::string md5Hash(const ByteArray &input);
    static std::string sha256Hash(const ByteArray &input);
    
};

class ServerSideEncryption {
public:
    void marshall( std::map<std::string,std::string> &headers ) {}
};

#endif

