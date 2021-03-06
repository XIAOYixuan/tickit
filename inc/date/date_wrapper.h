#ifndef TOMATO_DATE_WRAPPER_H
#define TOMATO_DATE_WRAPPER_H
#include <iostream>
#include <sstream>
#include <string>
#include "date/date.h"
#include "inc/util.h"

// TODO: don't expose namespace like this
using namespace std::chrono;
using namespace date;

namespace tomato {
using uint = unsigned int;
class DateW {
private:
    int year_;
    uint month_, day_;
    static DateW today_;
public:
    DateW(std::string text) {
        if (text == "today") {
            *this = DateW::today();
        } else {
            std::stringstream ss(text);
            ss >> year_ >> month_ >> day_;
        }
    }

    DateW(year_month_day ymd) {
        year_ = int(ymd.year());
        month_ = m_int(ymd.month());
        day_ = d_int(ymd.day());
    }

    DateW(int y, unsigned int m, unsigned int d) {
        year_ = y;
        month_ = m;
        day_ = d;
    }

    DateW(uint m, uint d) {
        auto today = year_month_day{floor<days>(system_clock::now())};
        year_ = (int)today.year();
        month_ = m;
        day_ = d;
    }

    DateW() {
        auto today = year_month_day{floor<days>(system_clock::now())};
        year_ = int(today.year());
        month_ = m_int(today.month());
        day_ = d_int(today.day());
    }

    inline int& wyear() {return year_;}
    inline uint& wmonth() {return month_;}
    inline uint& wday() {return day_;}
    // reprent the date in days, for rough cmp
    // TODO: check date.h, maybe there exists a similar method
    inline int int_for_cmp() const { 
        int ret = year_ * 365; 
        for (int i = 1; i+1 < month_; ++i) {
            ret += DateW::days_in_month_[i];
        }
        return ret + day_;
    }

    void print() {
        std::cout << year_ << " " << month_ << " " << day_ << std::endl;
    }

    std::string to_string() {
        // NOTE: don't change, if u must change, adjust to load book 
        // function as well
        // this func is used in dumpping node to taskbook
        return std::to_string(year_) + " " + std::to_string(month_)  
            + " " + std::to_string(day_);
    }

    std::string to_month_day() {
        return std::to_string(month_) + "." + std::to_string(day_);
    }

    std::string to_weekday() {
        auto wd = weekday{year{year_}/month_/day_};
        std::stringstream ss;
        ss << wd;
        return ss.str();
    }

    static DateW& today();

    static DateW to_date(std::string s);

    friend std::ostream& operator<<(std::ostream& os, const DateW& dt);
    friend DateW operator+(const DateW& lhs, uint days);
    friend DateW operator-(const DateW& lhs, uint days);
    friend int operator-(const DateW& lhs, const DateW& rhs);
    friend bool operator<(const DateW& lhs, const DateW& rhs);

private:
    uint m_int(month target) {
        for(uint i = 1; i < 13; ++i) {
            if (month{i} == target) return i;
        }
        CHECK(false) << "month not correct";
        return 0;
    }

    uint d_int(day target) {
        for (unsigned int i = 1; i < 32; ++i) {
            if (day{i} == target) return i;
        }
        CHECK(false) << "month not correct";
        return 0;
    }

    void add_one_day() {
        day_ ++;

        if (0 < day_ && day_ < 28) {
            return ;
        }

        if (month_ == 2 && day_ == 29) {
            if (year_ % 400 == 0) return ;
            if (year_ % 4 == 0 && year_ % 100 != 0) return ;
        } 

        if (day_ > DateW::days_in_month_[month_]) {
            day_ = 1;
            month_ ++;
            if (month_ == 13) {
                month_ = 1;
                year_ ++;
                CHECK(false) << "we reach a different year, not impl";
            }
            return ;
        }
        return ;
    }

    void minus_one_day() {
        day_ --;
        
        if (0 < day_ && day_ < 28) {
            return ;
        }
        
        if (month_ == 2 && day_ == 29) {
            if (year_ % 400 == 0) return ;
            if (year_ % 4 == 0 && year_ % 100 != 0) return ;
        } 

        if (day_ < 1) {
            month_ --;
            if (month_ < 1) {
                month_ = 12;
                year_ --;
                CHECK(false) << "we reach a different year, not impl";
            }
            day_ = DateW::days_in_month_[month_];
        }
    }

    static std::vector<int> days_in_month_;
};

DateW DateW::today_ = DateW();

DateW& DateW::today() {
    return DateW::today_;
}

DateW DateW::to_date(std::string s) {
    DateW ret;
    if (s == "today") {
        return DateW::today();
    } if (s == "to") {
        return DateW::today() + 1; 
    } else {
        auto nums = util::split(s, '.');
        CHECK(nums.size() == 2) << "original s :" << s;
        ret.year_ = DateW::today().wyear();
        ret.month_ = std::stoi(nums[0]);
        ret.day_ = std::stoi(nums[1]);
        return ret;
    }
}
std::vector<int> DateW::days_in_month_ = std::vector<int>{0, 31,28,31,30,31,30,31,31,30,31,30,31};
std::ostream& operator<<(std::ostream& os, const DateW& dt) {
        os << std::to_string(dt.year_) << "." << dt.month_ << "." << dt.day_;
        return os;
}

DateW operator+(const DateW& lhs, uint days) {
    auto ret = lhs;
    for (uint i = 1; i <= days; ++i) {
        ret.add_one_day();
    }
    return ret; 
}

DateW operator-(const DateW& lhs, uint days) {
    auto ret = lhs;
    for (uint i = 0; i < days; ++i) {
        ret.minus_one_day();
    }
    return ret; 
}

int operator-(const DateW& lhs, const DateW& rhs) {
    return lhs.int_for_cmp() - rhs.int_for_cmp();
}

bool operator<(const DateW& lhs, const DateW& rhs) {
    if (lhs.year_ < rhs.year_) return true;
    if (lhs.year_ > rhs.year_) return false;
    if (lhs.month_ < rhs.month_) return true;
    if (lhs.month_ > rhs.month_) return false;
    return lhs.day_ < rhs.day_;
}

// date and current time
class DateTime {
private:
    DateW date_;
    int sec_; // current time in second
public:
    // 2021 11 7 22:40:38
    DateTime(std::string systime) {
        auto text = util::split(systime);
        sec_ = util::time_to_sec(text.back());
        text.pop_back();
        date_ = DateW(util::join(text));
    }

private:
};

} // namespace tomato 
#endif // TOMATO_DATE_WRAPPER_H