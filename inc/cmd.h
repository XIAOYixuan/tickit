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
            CHECK(false) << "todo: vim task_id";
        } else if (text[0] == "lse") {
            CHECK(false) << "todo: list roadmap" << std::endl; 
        } else {
            std::cout << "unk cmd: [" << cmd << "]" << std::endl;
        }
        std::cout << ">> ";
    }

private:
    void list_info(std::vector<std::string>& text) {
        if (text.size() == 1) {
            // ls task
            ptask_handler_->print(text);
        } else if (text.size() == 2 && text[1] == TAG::epic) {
            pepic_handler_->print(text);
        } else if (text.size() == 2 && util::is_date(text[1])) {
            ptask_handler_->print(text);
        }
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H
