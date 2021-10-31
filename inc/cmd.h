#ifndef TOMATO_CMD_H
#define TOMATO_CMD_H

#include <iostream>
#include <string>
#include <cstdio>

#include "inc/util.h"
#include "inc/task.h"
#include "inc/calendar.h"
#include "inc/taskbook.h"
namespace tomato {

class CMD {
private:
    Calendar& calendar_;
    TaskBook& taskbook_;
public:
    CMD(Calendar& calendar, TaskBook& taskbook) 
        : calendar_(calendar), taskbook_(taskbook) {
    }
    
    void execute(const std::string& cmd) {
        if (util::startswith(cmd, "new")) {
            create_new_task(cmd);
        } else if (util::startswith(cmd, "newe")) {
            throw NotImplementedException(); 
        } 
        std::cout << ">> ";
    }

private:
    void create_new_epic(const std::string& cmd) {

    }

    void create_new_task(const std::string& cmd) {
        auto text = util::split(cmd);
        if (text.size() == 1) {
            // use vim
            throw NotImplementedException();
        } else {
            auto id = taskbook_.get_id();
            assert(text.size() >= 3);
            auto task = Task(id, text[1], util::join(text, 2));
            taskbook_.add_task(task);
        }
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H