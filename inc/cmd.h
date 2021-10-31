#ifndef TOMATO_CMD_H
#define TOMATO_CMD_H

#include <iostream>
#include <string>
#include <cstdio>

#include "inc/util.h"
#include "inc/task.h"
#include "inc/calendar.h"
namespace tomato {

class CMD {
private:
    Calendar& calendar_;

public:
    CMD(Calendar& calendar) : calendar_(calendar) {
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
            assert(text.size() >= 3);
            auto task = Task(text[1], util::join(text, 2));
        }
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H