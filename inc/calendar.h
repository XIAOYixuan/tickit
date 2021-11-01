#ifndef TOMATO_CALENDAR_H
#define TOMATO_CALENDAR_H
#include <string>
#include <vector>
#include "inc/util.h"
#include "inc/task.h"
#include "inc/taskbook.h"
namespace tomato {
class Day {
private:
    std::vector<TaskPtr> ptasks_;
public:
    void add_task(TaskPtr ptask) {
        ptasks_.push_back(ptask);
    }

    std::vector<TaskPtr>& tasks() { return ptasks_; }
};
using DayPtr = std::shared_ptr<Day>;

class Week {
};

class Month {
    std::vector<DayPtr> pdays_;
public:
    Month() {
        for(int i = 0; i < 32; ++i) {
            pdays_.push_back(DayPtr(new Day()));
        }
    }
    void add_task(TaskPtr ptask) {
        pdays_[ptask->date().wday()]->add_task(ptask);
    }

    std::vector<TaskPtr>& get_tasks(int day) { 
        return pdays_[day]->tasks();
    }
};

class Calendar {
private:
    std::vector<Month> months_;

public:
    Calendar(){
        for(int i = 0; i < 13; ++i) {
            months_.push_back(Month());
        }
    };
    ~Calendar(){};

    void load_tasks(TaskBook& taskbook) {
        for(auto ptask : taskbook.tasks()) {
            add_task(ptask);
        }
    }

    void add_task(TaskPtr ptask) {
        check_date(ptask->date());
        months_[ptask->date().wmonth()].add_task(ptask);
    }

    std::vector<TaskPtr>& get_tasks(DateW& dt) {
        check_date(dt);
        return months_[dt.wmonth()].get_tasks(dt.wday());
    }

private:
    inline void check_date(DateW& dt) {
        // TODO: extent to more years? only allow one year now
        CHECK(dt.wyear() == DateW::today().wyear());
    }
};

}
#endif // TOMATO_CALENDAR_H