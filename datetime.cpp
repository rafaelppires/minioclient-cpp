#include <datetime.h>
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

    struct tm gmtimestamp;
    std::gmtime_r(&time_,&gmtimestamp);
    if (std::strftime(tstr, sizeof(tstr), format, &gmtimestamp)) {
        return tstr;
    }

    return "";
}

//------------------------------------------------------------------------------
DateTime DateTime::parseDateTime(const std::string &) { return DateTime(); }

//------------------------------------------------------------------------------
