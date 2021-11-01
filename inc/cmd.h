#ifndef TOMATO_CMD_H
#define TOMATO_CMD_H

#include <iostream>
#include <string>
#include <cstdio>

#include "inc/util.h"
#include "inc/task.h"
#include "inc/calendar.h"
#include "inc/taskbook.h"
#include "inc/info_printer.h"
#include "inc/date/date_wrapper.h"
namespace tomato {

class CMD {
private:
    Calendar& calendar_;
    TaskBook& taskbook_;

public:
    CMD(Calendar& calendar, TaskBook& taskbook) 
        : calendar_(calendar), taskbook_(taskbook) {
        calendar_.load_tasks(taskbook_);
    }
    
    void execute(const std::string& cmd) {
        auto text = util::split(cmd);
        if (util::startswith(text[0], "new")) {
            create_new_task(text);
        } else if (util::startswith(text[0], "newe")) {
            throw NotImplementedException(); 
        } else if (text[0] == "ls") {
            list_info(text);
        }
        std::cout << ">> ";
    }

private:
    void list_info(std::vector<std::string>& text) {
        if (text.size() == 1) {
            InfoPrinter infop(calendar_, DateW::today());
        } else {
            throw NotImplementedException();
        }
    }

    void create_new_task(std::vector<std::string>& text) {
        if (text.size() == 1) {
            // use vim
            throw NotImplementedException();
        } else {
            auto id = taskbook_.get_id();
            CHECK(text.size() >= 3);
            TaskPtr ptask(new Task(id, DateW::today(), text[1], util::join(text, 2)));
            taskbook_.add_task(ptask);
            calendar_.add_task(ptask);
        }
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H