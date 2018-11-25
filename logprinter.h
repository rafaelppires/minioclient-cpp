#ifndef _LOG_PRINTER_H_
#define _LOG_PRINTER_H_

#include <ostream>
#include <string>
#include <libc_mock/libc_proxy.h>

extern "C" { int printf(const char*,...); }
namespace std {
const std::string endl = "\n";
template <class charT>
class basic_ostream {
public:
    basic_ostream& operator<<(const std::string &s) {
        printf("%s",s.c_str());
    }
};

}

class LogPrinter {
public:
    LogPrinter(std::basic_ostream<char> &stream) : stream_(stream) {}

    void println(const std::string &line) {
        stream_ << line + std::endl;
    }
private:
    std::basic_ostream<char> &stream_;
};

#endif

