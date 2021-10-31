#ifndef TOMATO_CMD_H
#define TOMATO_CMD_H

#include <iostream>
#include <string>
#include <cstdio>
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
        if (cmd == "new") {
        } else {

        }
        std::cout << ">> ";
    }

private:
    void create_new_task() {
        auto task = Task();
        std::string tmp_md = "temp.today.md";
        auto callvim = "vim " + tmp_md;
        system(callvim.c_str());
        task.parse_markdown(tmp_md);
        task.show();
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H