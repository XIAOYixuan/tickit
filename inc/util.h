#ifndef TOMATO_UTIL_H
#define TOMATO_UTIL_H
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <limits>
#include <glog/logging.h>

namespace tomato {

class NotImplementedException : public std::logic_error {
public:
    NotImplementedException () : std::logic_error{
        "Function not yet implemented."} {}
};

namespace util {
inline bool startswith(const std::string& original, 
    const std::string& target) {
    return original.rfind(target, 0) == 0;
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_date(const std::string& s) {
    // tomorrow
    if (s == "to") {
        return true;
    } else if (s.size() > 2 && (s.find(".") != std::string::npos)) {
        // TODO: regex
        return true;
    } else {
        return false;
    }
}

std::vector<std::string> split(const std::string& text, char delim=' ') {
    std::vector<std::string> ret;
    std::stringstream ss(text);
    std::string token;
    if (delim == ' ') {
        while(ss >> token) {
            ret.push_back(token);
        }
    } else {
        while (std::getline(ss, token, delim)) {
            ret.push_back(token);
        }
    }
    return std::move(ret);
}

std::string join(std::vector<std::string>& text, size_t st=0, 
    size_t ed=std::numeric_limits<size_t>::max()) {
    std::stringstream ss;
    st = std::max((size_t)0, st);
    ed = std::min(text.size(), ed);
    for (int i = st; i < ed; ++i) {
        ss << text[i];
        if (i < ed-1) {
            ss << " ";
        }
    }
    return ss.str();
}



} // end namespace util
} // end namespace tomato
#endif // TOMATO_UTIL_H