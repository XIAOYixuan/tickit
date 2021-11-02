#ifndef TOMATO_UTIL_H
#define TOMATO_UTIL_H
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <limits>
#include <regex>
#include <iterator>
#include <glog/logging.h>

namespace tomato {

class NotImplementedException : public std::logic_error {
public:
    NotImplementedException () : std::logic_error{
        "Function not yet implemented."} {}
};

namespace util {
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

// TODO: time class
int time_to_int(std::string t) {
    if (t == "---") {
        return std::numeric_limits<int>::max();
    } 
    int add = 0;
    if (t.find("am") != std::string::npos) {
        t.pop_back();
        t.pop_back();
    } 
    if (t.find("pm") != std::string::npos) {
        t.pop_back();
        t.pop_back();
        add = 12;
    }
    auto hm = split(t, ':');
    int h = std::stoi(hm[0]);
    int m = std::stoi(hm[1]);
    return (h+add)*60 + m;
}

 void extract_leading_time_one(std::string& s, std::string& ltime) {
     std::regex plain("(\\d{1,2}:\\d{1,2})");
     std::regex withm("(\\d{1,2}:\\d{1,2})(am|pm)");
     std::smatch mobj;
 
     std::regex_search(s, mobj, withm);
     if (mobj.position() == 0) {
         int split = mobj.position();
         ltime = s.substr(split, split+mobj.length());
         s = s.substr(split+mobj.length()+1);
         return;
     }
 
     std::regex_search(s, mobj, plain);
     if (mobj.position() == 0) {
         int split = mobj.position();
         ltime = s.substr(split, split+mobj.length());
         s = s.substr(split+mobj.length()+1);
         return;
     }
 
     ltime = "";
     return ;
 }
 
 void extract_leading_time(std::string& s, std::string& start, std::string& end) {
     extract_leading_time_one(s, start);
     extract_leading_time_one(s, end);
     if (start.empty()) { start = "---";}
     if (end.empty()) { end = "---";}
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