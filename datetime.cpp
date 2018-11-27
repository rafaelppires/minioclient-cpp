#include <datetime.h>
//------------------------------------------------------------------------------
#ifdef ENCLAVED
struct tm* std::gmtime(std::time_t *in) {
    static struct tm ret;
    ::sgx_gmtime_r(in, &ret);
    return &ret;
} 
#endif
//------------------------------------------------------------------------------
std::string DateTime::toString(DateFormat::Format f) {
    char tstr[100];
    const char *format;
    switch (f) {
        case DateFormat::SIGNER_DATE_FORMAT:
            format = "%Y%m%d";
            break;
        case DateFormat::AMZ_DATE_FORMAT:
            format = "%Y%m%dT%H%M%SZ";
            break;
    };

    if (std::strftime(tstr, sizeof(tstr), format, std::gmtime(&time_))) {
        return tstr;
    }

    return "";
}

//------------------------------------------------------------------------------
DateTime DateTime::parseDateTime(const std::string &) { return DateTime(); }

//------------------------------------------------------------------------------
