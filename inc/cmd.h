#ifndef TOMATO_CMD_H
#define TOMATO_CMD_H

#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include "inc/util.h"
#include "inc/task.h"
#include "inc/calendar.h"
#include "inc/taskbook.h"
#include "inc/info_printer.h"
#include "inc/date/date_wrapper.h"
#include "inc/cmd_handler.h"
namespace tomato {

class CMD {
private:
    Calendar& calendar_;
    TaskBook& taskbook_;
    std::shared_ptr<EpicHandler> pepic_handler_;
    std::shared_ptr<TaskHandler> ptask_handler_;

public:
    CMD(Calendar& calendar, TaskBook& taskbook) 
            : calendar_(calendar), taskbook_(taskbook) {
        calendar.load_tasks(taskbook);
        ptask_handler_.reset(new TaskHandler(calendar, taskbook));
        pepic_handler_.reset(new EpicHandler(calendar, taskbook));
    }
    
    void execute(const std::string& cmd) {
        // TODO: check cmd grammar
        auto text = util::split(cmd);
        if (util::startswith(text[0], "enew")) {
            pepic_handler_->create(text);
        } else if (util::startswith(text[0], "new")) {
            ptask_handler_->create(text);
        } else if (text[0] == "ls") {
            list_info(text);
        } else if (text[0] == "rp") {
            ptask_handler_->repeat(text);
        } else if (text[0] == "save" || text[0] == "w" || text[0] == "wq") {
            taskbook_.save();
        } else if (text[0] == "vim") {
            ptask_handler_->edit(text);
        } else if (text[0] == "lse") {
            CHECK(false) << "todo: list roadmap" << std::endl; 
        } else if (text[0] == "tick") {
            ptask_handler_->tick(text);
        } else if (text[0] == "touch") {
            ptask_handler_->touch(text);
        } else if (text[0] == "ref") {
            // refresh
            ptask_handler_->refresh(text);
        } else if (text[0] == "drop") {
            // drop id
            ptask_handler_->drop(text);
        } else if (text[0] == "aktiv") {
            ptask_handler_->aktiv(text);
        } else if (text[0] == "mv") {
            // mv id1 id2 id3 date
            ptask_handler_->move(text);
        } else if (text[0] == "time") {
            // time id start time
            ptask_handler_->set_time(text);
        } else if (text[0] == "stat") {
            // analyze info
            stat_info(text);
        } else if (text[0] == "repe") {
            // show the epics whose latest day is 2 days algo.1
            pepic_handler_->repe(text);
        } else if (text[0] == "pomo") {
            ptask_handler_->pomodora(text);
        } else if (text[0] == "rm") {
            ptask_handler_->rm(text);
        } else if (text[0] == "drope") {
            pepic_handler_->drop(text);
        } else {
            std::cout << "unk cmd: [" << cmd << "]" << std::endl;
        }
        std::cout << ">> ";
    }

private:
    void stat_info(std::vector<std::string>& cmd) {
        ptask_handler_->stat_week(cmd);
    }    

    void list_info(std::vector<std::string>& text) {
        if (text.size() == 1) {
            // ls task
            ptask_handler_->print(text);
        } else if (text.size() == 2 && text[1] == TAG::epic) {
            // ls epic
            pepic_handler_->print(text);
        } else if (text.size() == 2 && util::is_date(text[1])) {
            // ls specific day
            ptask_handler_->print(text);
        } else if (text.size() == 2 && text[1] == "arch") {
            ptask_handler_->print_arch(text);
        } else if (text[1] == "week") {
            ptask_handler_->print_this_week(text);
            // ls duration: ls week, ls month, todo
        }
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H
