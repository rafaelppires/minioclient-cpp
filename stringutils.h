#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <httpcommon.h>

namespace StringUtils {

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
