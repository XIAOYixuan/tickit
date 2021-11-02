#ifndef TOMATO_INFO_PRINTER_H
#define TOMATO_INFO_PRINTER_H
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "inc/util.h"
#include "inc/calendar.h"
#include "inc/taskbook.h"
#include "inc/date/date_wrapper.h"
#include "tabulate/table.hpp"
#include "tabulate/markdown_exporter.hpp"
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

class TaskMarkDown {
private:
    TaskPtr ptask_;
    std::string path_;
    
public:
    TaskMarkDown(TaskPtr ptr, 
        std::string path = TEMPLATE::task_path) {
        ptask_ = ptr;
        
        auto id = std::to_string(ptask_->id());
        auto title = ptask_->title();
        std::replace(title.begin(), title.end(), ' ', '_');
        path_ = path + "/" + id + "_" + title + ".md";

        create_md();
    }

    inline std::string& path() {
        return path_;
    }

private:
    void create_md() {
        std::ifstream fin(path_);
        if (fin.good()) return;
        
        std::ofstream fout(path_);
        fout << intro_string() << std::endl;
        fout.close();
    }

    std::string intro_string() {
        // table_.add_row({"Title", ptask_->title()});
        // table_.add_row({"Epic", ptask_->title()});
        // table_.add_row({"Date", ptask_->date().to_string()});
        // add hashtag/aka label

        // tabulate::MarkdownExporter exporter;
        // auto md = exporter.dump(table_);
        std::stringstream ss;
        // ###Title: 
        // epic:xx
        // created-date:...
        // ---
        ss <<"# Title: " << ptask_->title() << std::endl;
        ss << ptask_->date().to_string() << std::endl;
        ss << "epic: " << ptask_->epic() << std::endl;
        ss << std::endl;
        ss << "---" << std::endl;
        
        ss <<"#### Description" << std::endl;
        return ss.str();
    }
};

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
        tabulate::Table title;
        std::stringstream ss;
        ss << date_ << " " << date_.to_weekday();
        title.add_row({ss.str()});
        title.format()
            .font_style({tabulate::FontStyle::underline})
            .font_align(tabulate::FontAlign::center)
            .font_background_color(tabulate::Color::grey)
            .hide_border_top()
            .hide_border_left()
            .hide_border_bottom()
            .hide_border_right();
        std::cout << title << std::endl;

        std::vector<TaskPtr> ptasks = calendar_.get_tasks(date_);
        table_.add_row({TAG::id, TAG::title, TAG::status, TAG::epic, 
            TAG::start, TAG::end});

        std::sort(std::begin(ptasks), std::end(ptasks), [] (auto& lhs, auto& rhs) {
            if (lhs->status() != "done" && rhs->status() == "done") {
                return true;
            } else if (lhs->start_int() != rhs->start_int()) {
                return lhs->start_int() < rhs->start_int();
            } else {
                return lhs->end_int() < rhs->end_int();
            }
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
