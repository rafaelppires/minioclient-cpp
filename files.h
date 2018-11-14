#ifndef _FILES_H_
#define _FILES_H_

#include <fstream>
#include <vector>

class Files {
   public:
    static size_t size(const std::string &fname) {
        std::ifstream in(fname.c_str(),
                         std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    static std::vector<char> readAll(const std::string fname) {
        std::ifstream file(fname.c_str(), std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            throw std::runtime_error("could not read file '" + fname + "'");
        }
        return buffer;
    }
};

#endif
