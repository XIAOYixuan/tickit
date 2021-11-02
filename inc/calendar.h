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

    std::vector<DayPtr>& days() { return pdays_;}
};

class Calendar {
private:
    std::vector<Month> months_;
    std::vector<TaskPtr> archive_;
public:
    Calendar(){
        for(int i = 0; i < 13; ++i) {
            months_.push_back(Month());
        }
    };
    ~Calendar(){};

    void load_tasks(TaskBook& taskbook) {
        for(auto ptask : taskbook.tasks()) {
            if (ptask->status() == VALUE::arch) {
                archive_.push_back(ptask);
            } else {
                add_task(ptask);
            }
        }
    }

    void drop_task(TaskPtr ptask) {
        delete_task(ptask);
        archive_.push_back(ptask);
    }

    void aktiv_task(TaskPtr ptask) {
        size_t del_point = archive_.size();
        for (size_t i = 0; i < archive_.size(); ++i) {
            if (archive_[i] == ptask) {
                del_point = i;
                break;
            }
        }
        
        if (del_point == archive_.size()) {
            LOG(INFO) << "can't found the task " 
                << ptask->id() << " in the date: " << ptask->date(); 
            return;
        }
        archive_.erase(archive_.begin()+del_point);
        add_task(ptask);
    }

    void move_date(TaskPtr ptask, DateW new_date) {
         delete_task(ptask);
         ptask->set_date(new_date);
         add_task(ptask);
    }

    // TODO: directly do it here
    void add_task(TaskPtr ptask) {
        check_date(ptask->date());
        months_[ptask->date().wmonth()].add_task(ptask);
    }

    std::vector<TaskPtr>& get_tasks(DateW& dt) {
        check_date(dt);
        return months_[dt.wmonth()].get_tasks(dt.wday());
    }

    inline std::vector<TaskPtr>& get_arch() { return archive_;}

private:
    void delete_task(TaskPtr ptask) {
        auto date = ptask->date(); 
        auto& the_month = months_[date.wmonth()];
        auto& the_day = the_month.days()[date.wday()];
        auto& all_tasks = the_day->tasks();
        size_t del_point = all_tasks.size(); 
        for (size_t i = 0; i < all_tasks.size(); ++i) {
            if (all_tasks[i] == ptask) {
                del_point = i;
                break;
            }
        }
        if (del_point == all_tasks.size()) {
            LOG(INFO) << "can't found the task " 
                << ptask->id() << " in the date: " << ptask->date(); 
            return;
        }
        all_tasks.erase(all_tasks.begin()+del_point);
    }

    inline void check_date(DateW& dt) {
        // TODO: extent to more years? only allow one year now
        CHECK(dt.wyear() == DateW::today().wyear());
    }
};

}
#endif // TOMATO_CALENDAR_H