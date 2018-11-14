#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_ 

class DateFormat {
public:
    enum Format {
        SIGNER_DATE_FORMAT,
        AMZ_DATE_FORMAT
    };
};

class DateTime {
   public:
    std::string toString( DateFormat::Format ) { return "10_pra_daqui_a_pouco"; }

    static DateTime parseDateTime(const std::string &) { return DateTime(); }
};

#endif

