#ifndef TOMATO_INFO_PRINTER_H
#define TOMATO_INFO_PRINTER_H
#include <iostream>
#include <string>
#include <algorithm>
#include "inc/util.h"
#include "inc/calendar.h"
#include "inc/taskbook.h"
#include "inc/date/date_wrapper.h"
#include "tabulate/table.hpp"

namespace tomato {
class InfoPrinter {
public:
    InfoPrinter() {}

protected:
    tabulate::Table table_;
    virtual void list_info() = 0;
    friend std::ostream& operator<<(std::ostream& os, const InfoPrinter& tb);
};

std::ostream& operator<<(std::ostream& os, const InfoPrinter& tb) {
    os << tb.table_;
    return os;
}

class InfoTask : public InfoPrinter {
private:
    Calendar& calendar_;
    DateW& date_;

public:
    InfoTask(Calendar& calendar, DateW& dt) 
        : calendar_(calendar), date_(dt) {
            list_info();
    }

protected:
    void list_info() override {
        std::cout << date_ << std::endl;
        std::vector<TaskPtr> ptasks = calendar_.get_tasks(date_);

        table_.add_row({TAG::id, TAG::title, TAG::status, TAG::epic, 
            TAG::start, TAG::end});

        std::sort(std::begin(ptasks), std::end(ptasks), [] (auto& lhs, auto& rhs) {
            return lhs->status() != "done" && rhs->status() == "done";
        }); 

        table_[0].format().font_style({tabulate::FontStyle::bold}).font_align(tabulate::FontAlign::center);
        for (size_t i = 0; i < ptasks.size(); ++i) {
            auto& ptask = ptasks[i];
            table_.add_row(
                {std::to_string(ptask->id()), 
                    ptask->title(),
                    ptask->status(),  
                    ptask->epic(), 
                    ptask->start(), ptask->end()});
            if (ptask->status()  == "done") {
                table_[i+1].format().font_color(tabulate::Color::green);
            }
        }
        std::cout << table_ << std::endl;
    }
};

class InfoEpic : public InfoPrinter {
private:
    TaskBook tb_;

public: 
    InfoEpic(TaskBook& tb) : tb_(tb) {
        list_info();
    }

protected:
    void list_info() override {
        std::vector<std::pair<std::string, int>> epics;

        for (auto& it : tb_.epics()) {
            epics.push_back(std::make_pair(it.first, it.second->id()));
        }

        std::sort(epics.begin(), epics.end());

        table_.add_row({TAG::id, TAG::title});
        for (auto& it : epics) {
            table_.add_row(
                {std::to_string(it.second), it.first});
        }
        table_[0].format().font_style({tabulate::FontStyle::bold});
        std::cout << table_ << std::endl;
    }
};

} // namespace tomato

#endif // TOMATO_INFO_PRINTER_H
