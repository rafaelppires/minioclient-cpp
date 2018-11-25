#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_ 

#include <ctime>
#include <string>


#ifdef ENCLAVED
#include <libc_mock/libc_proxy.h>
#include <ssl_wrappers.h>
#define gmtime_r sgx_gmtime_r
#define time sgx_time
namespace std {
using ::sgx_time;
using ::sgx_gmtime_r;
}
#endif

class DateFormat {
public:
    enum Format {
        SIGNER_DATE_FORMAT,
        AMZ_DATE_FORMAT
    };
};

class DateTime {
   public:
    DateTime() : time_(std::time(nullptr)) {}
    std::string toString( DateFormat::Format );

    static DateTime parseDateTime(const std::string &);

   private:
    std::time_t time_;
};

#endif

