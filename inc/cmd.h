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
            create_new_epic(text);
            throw NotImplementedException(); 
        } else if (text[0] == "ls") {
            list_info(text);
        }
        std::cout << ">> ";
    }

private:
    void list_info(std::vector<std::string>& text) {
        if (text.size() == 1) {
            InfoTask infop(calendar_, DateW::today());
        } else if (text.size() == 2 && text[1] == TAG::epic) {
            InfoEpic infoe(taskbook_);
        } else {
            throw NotImplementedException();
        }
    }

    void create_new_task(std::vector<std::string>& text) {
        if (text.size() == 1) {
            // use vim
            throw NotImplementedException();
        } else if (taskbook_.epic_exist(text[1])) {
            auto id = taskbook_.get_id();
            CHECK(text.size() >= 3);
            TaskPtr ptask(new Task(id, DateW::today(), text[1], util::join(text, 2)));
            taskbook_.add_task(ptask);
            calendar_.add_task(ptask);
        } else {
            // TODO: enable epics with spaces in between
            std::cout << "Epic [" + text[1] + "] does not exist, the similar one could be: " << std::endl;
            // TODO: find similar ones, med
            std::cout << "would you like to create one? [y/n]" << std::endl;
            std::cout << ">> ";
            std::string response;
            while (std::cin >> response) {
                if (response == "y") {
                    std::vector<std::string> epic_cmd({"newe", text[1]});
                    create_new_epic(epic_cmd);
                    create_new_task(text);
                    break;
                } else if (response == "n") {
                    std::cout << "end process" << std::endl;
                    break;
                } else {
                    std::cout << "please enter [y/n]" << std::endl;
                }
            }
        }
    }

    void create_new_epic(std::vector<std::string>& text) {
        CHECK(text.size()>1);

        auto id = taskbook_.get_epic_id();
        EpicPtr pepic(new Epic(id, DateW::today(), util::join(text, 1)));
        taskbook_.add_epic(pepic);
    }
};

} // end namespace tomato

#endif // TOMATO_CMD_H