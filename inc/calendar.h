#ifndef TOMATO_CALENDAR_H
#define TOMATO_CALENDAR_H
#include <string>
#include <vector>
#include "util.h"
namespace tomato {
class Day {
};

class Week {
};

class Month {
};

class Calendar {
private:
    std::string year = "2021";
    std::vector<Month> months;
public:
    Calendar(){
    };
    ~Calendar(){};

};

}
#endif // TOMATO_CALENDAR_H