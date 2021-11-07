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
using Row_t = std::vector<variant<std::string, const char *, tabulate::Table>>;
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

class TaskPrinter : public InfoPrinter {
protected:
    Calendar& calendar_;
    std::unordered_map<std::string, EpicPtr>& epic_map_;
    // TODO: optimizable
    std::vector<TaskPtr> ptasks_;

public:
    TaskPrinter(Calendar& calendar, 
        std::unordered_map<std::string, EpicPtr>& epic_map) 
        : calendar_(calendar), epic_map_(epic_map) {
    }

    inline std::string id_epic(std::string epic) {
        auto epic_id = epic_map_.at(epic)->id();
        return std::to_string(epic_id) + ":" + epic;
    }
    
    void list_info() override {

        auto& ptasks = ptasks_;
        table_.add_row({TAG::id, TAG::title, TAG::status, TAG::epic, 
            TAG::start, TAG::end});

        std::sort(std::begin(ptasks), std::end(ptasks), [] (auto& lhs, auto& rhs) {
            if (lhs->status() != "done" && rhs->status() == "done") {
                return true;
            } else if (lhs->status() != "done" && rhs->status() != "done") {
                if (lhs->start_int() != rhs->start_int()) {
                    return lhs->start_int() < rhs->start_int();
                } else {
                    return lhs->end_int() < rhs->end_int();
                } 
            } else {
                return false;
            }
        }); 

        table_[0].format().font_style({tabulate::FontStyle::bold}).font_align(tabulate::FontAlign::center);
        for (size_t i = 0; i < ptasks.size(); ++i) {
            auto& ptask = ptasks[i];
            table_.add_row(
                {std::to_string(ptask->id()), 
                    ptask->title(),
                    ptask->status(), 
                    id_epic(ptask->epic()),
                    ptask->start(), ptask->end()});
            if (ptask->status()  == "done") {
                table_[i+1].format().font_color(tabulate::Color::green);
            }
        }
        // table_.column(3).format().width(15);
        std::cout << table_ << std::endl;
    }
};

class InfoTask : public TaskPrinter{
private:
    DateW& date_;
public:
    InfoTask(Calendar& calendar, 
        std::unordered_map<std::string, EpicPtr>& epic_map,
        DateW& dt) 
            : TaskPrinter(calendar, epic_map), date_(dt) {
        ptasks_ = calendar_.get_tasks(date_);
        
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
        
        list_info();
    }
};

class InfoArch : public TaskPrinter {
public:
    InfoArch(Calendar& calendar,
        std::unordered_map<std::string, EpicPtr>& epic_map)
            : TaskPrinter(calendar, epic_map) {
        ptasks_ = calendar_.get_arch();
        list_info();
    }
};

// TODO: maybe another stat printer
class StatTask : public TaskPrinter {
private:
    std::vector<DateW>& days_;
    std::unordered_map<int, std::vector<TaskPtr>> epic_tasks_;
    std::vector<std::pair<int, std::string>> epics_;

public:
    StatTask(Calendar& calendar,
        std::unordered_map<std::string, EpicPtr>& epic_map, 
        std::vector<DateW>& days)
            : TaskPrinter(calendar, epic_map), days_(days) {
        get_all_tasks();
        group_by_epics();
        list_info();
        output_to_python();
    }

private:
    void output_to_python() {
        // TODO: directly call python
        std::ofstream fpypath("stat/epic.tmp");
        for (auto& epic : epics_) {
            fpypath << epic.first << " "  << epic.second << " ";
            auto& ptasks = epic_tasks_.at(epic.first);
            fpypath << ptasks.size() << std::endl;
            for (auto& task : ptasks) {
                fpypath << task->title() << std::endl;
            }
        }
        fpypath.close();
        system("python stat/plot.py stat/epic.tmp");
    }

    void get_all_tasks() {
        for (auto& oneday : days_) {
            auto& ptasks = calendar_.get_tasks(oneday);
            ptasks_.insert(ptasks_.end(), ptasks.begin(), ptasks.end());
        }
    }

    void group_by_epics() {
        for (auto& ptask : ptasks_) {
            auto epic = ptask->epic();
            auto epic_id = epic_map_.at(epic)->id();
            if (epic_tasks_.count(epic_id) == 0) {
                epic_tasks_[epic_id] = std::vector<TaskPtr>{ptask};
                epics_.push_back(std::make_pair(epic_id, epic));
            } else {
                epic_tasks_.at(epic_id).push_back(ptask);
            }
        }
        std::sort(epics_.begin(), epics_.end());
    } 
    
    void list_info() override {
        for (size_t i = 0; i < epics_.size(); ++i) {
            list_one_epic(id_epic(epics_[i].second), 
                epic_tasks_[epics_[i].first]);
        }
    }

    void list_one_epic(std::string epic, std::vector<TaskPtr>& ptasks) {
        tabulate::Table header;
        header.add_row(Row_t{epic});
        header[0].format()
            .hide_border()
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::blue)
            .width(102);

        tabulate::Table table;
        // table.add_row({epic});
        // table.add_row({TAG::id, TAG::title, TAG::status, TAG::start, TAG::end});
        table.add_row({TAG::id, TAG::date, TAG::title, TAG::status});
        std::sort(ptasks.begin(), ptasks.end(), [](auto& lhs, auto& rhs){
            // return lhs->id() < rhs->id(); 
            return lhs->date().int_for_cmp() < rhs->date().int_for_cmp();
        });
        for(size_t i = 0; i < ptasks.size(); ++i) {
            auto& ptask = ptasks[i];
            table.add_row({std::to_string(ptask->id()), 
                ptask->date().to_month_day(), 
                ptask->title(), 
                ptask->status()}
            );
            if (ptask->status() == VALUE::done) {
                table[i+1].format().font_color(tabulate::Color::green);
            }
        }
        table[0].format().background_color(tabulate::Color::grey);
        table.column(0).format().width(4);
        table.column(1).format().width(10);
        table.column(2).format().width(80);
        table.column(3).format().width(8);
        // header.add_row(Row_t{table});
        std::cout << header << std::endl;
        std::cout << table << std::endl;
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

// TODO: what a mess here
class InfoRepe : public InfoPrinter {
private:
    TaskBook& taskbook_;
    int gap_;
public:
    InfoRepe(TaskBook& tb, int gp) 
        : taskbook_(tb), gap_(gp) {
        list_info();
    }

protected:
    void list_info() override {
        std::unordered_map<std::string, DateW> latest_tasks;
        for (auto& ptask : taskbook_.tasks()) {
            if (ptask->status() != VALUE::done) continue;
            auto cur_epic = ptask->epic();
            if (latest_tasks.count(cur_epic) == 0) {
                latest_tasks[cur_epic] = ptask->date();
            } else if (latest_tasks.at(cur_epic) < ptask->date()) {
                latest_tasks[cur_epic] = ptask->date();
            }
        }
        table_.add_row({TAG::id, TAG::epic, TAG::date, "gap"});
        auto today = DateW::today();
        for (auto& it : latest_tasks) {
            int gap = today - it.second;
            if (gap <= gap_) continue;
            int epic_id = taskbook_.epics().at(it.first)->id();
            table_.add_row({
                std::to_string(epic_id), 
                it.first, 
                it.second.to_month_day(), 
                std::to_string(gap)
                });
        }
        table_[0].format().font_style({tabulate::FontStyle::bold});
        std::cout << table_ << std::endl;

    }
};
} // namespace tomato

#endif // TOMATO_INFO_PRINTER_H
