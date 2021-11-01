#ifndef TOMATO_DATE_WRAPPER_H
#define TOMATO_DATE_WRAPPER_H
#include <iostream>
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

public:
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

    void print() {
        std::cout << year_ << " " << month_ << " " << day_ << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const DateW& dt);

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
};

std::ostream& operator<<(std::ostream& os, const DateW& dt) {
        os << dt.year_ << "/" << dt.month_ << "/" << dt.day_;
        return os;
}

} // namespace tomato 
#endif // TOMATO_DATE_WRAPPER_H