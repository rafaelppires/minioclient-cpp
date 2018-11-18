#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <httpcommon.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>


namespace StringUtils {

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

class Joiner;
class MapJoiner {
   public:
    MapJoiner(const Joiner &j, const std::string &kvsep)
        : joiner_(j), kvSeparator_(kvsep) {}

    std::string join(const KeyValueMap &map);

   private:
    std::string kvSeparator_;
    const Joiner &joiner_;
};

class Joiner {
   public:
    Joiner(const std::string &sep) : separator_(sep) {}
    MapJoiner withKeyValueSeparator(const std::string &kvSeparator) {
        return MapJoiner(*this, kvSeparator);
    }

    template <typename T>
    std::string join(const T &collection) const {
        std::string ret;
        for (const auto &it : collection) {
            ret += it + separator_;
        }

        if (!ret.empty())
            ret.erase(ret.size() - separator_.size(),
                      separator_.size());
        return ret;
    }

    static Joiner on(const std::string &separator) { return Joiner(separator); }

   private:
    std::string separator_;
};

std::vector<std::string> split(const std::string &str, const std::string &sep);
std::string replaceAll(std::string str, const std::string &from,
                       const std::string &to);
std::string tolower(std::string s);
}  // namespace StringUtils

#endif
