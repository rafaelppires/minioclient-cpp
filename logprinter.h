#ifndef _LOG_PRINTER_H_
#define _LOG_PRINTER_H_

#include <ostream>

class LogPrinter {
public:
    LogPrinter(std::basic_ostream<char> &stream) : stream_(stream) {}

    void println(const std::string &line) {
        stream_ << line << std::endl;
    }
private:
    std::basic_ostream<char> &stream_;
};

#endif

