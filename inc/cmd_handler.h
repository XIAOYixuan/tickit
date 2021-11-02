#ifndef TOMATO_CMD_HANDLER_H
#define TOMATO_CMD_HANDLER_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "inc/util.h"
#include "inc/taskbook.h"
#include "inc/calendar.h"
#include "inc/info_printer.h"
#include "inc/cmd_parser.h"
namespace tomato {

class BaseCMDHandler {
protected:
    Calendar& calendar_;
    TaskBook& taskbook_;
    CMDParser parser_;
public:
    BaseCMDHandler(Calendar& c, TaskBook& t) 
        : calendar_(c), taskbook_(t) {}
    virtual void create(std::vector<std::string>& cmd) = 0;
    virtual void print(std::vector<std::string>& cmd) = 0;
};

class EpicHandler : public BaseCMDHandler {
public:
    EpicHandler(Calendar& c, TaskBook& t) : BaseCMDHandler(c, t) {}
    void create(std::vector<std::string>& cmd) override {
        CHECK(cmd.size()>1);

        auto id = taskbook_.get_epic_id();
        EpicPtr pepic(new Epic(id, DateW::today(), util::join(cmd, 1)));
        taskbook_.add_epic(pepic);
    }

    void print(std::vector<std::string>& cmd) override {
        InfoEpic info(taskbook_);
    }

};

class TaskHandler : public BaseCMDHandler {
    std::string head_, time_, epic_, text_;
public:
    TaskHandler(Calendar& c, TaskBook& t) : BaseCMDHandler(c, t) {}
using cmdvec = std::vector<std::string>;
    void drop(cmdvec& cmd) {
        if (cmd.size() != 2 || !util::is_number(cmd[1])) {
            LOG(INFO) << "invalid cmd : " << util::join(cmd);
        }
        auto ptask = taskbook_.get_task_by_id(cmd[1]);
        if (ptask == nullptr) {
            LOG(INFO) << "unkwnon id : " << cmd[1];
        }

        calendar_.drop_task(ptask);
        ptask->set_status(VALUE::arch);
    }

    void edit(std::vector<std::string>& cmd) {
        auto id = std::stoi(cmd[1]);
        auto ptask = taskbook_.get_task_by_id(id);
        TaskMarkDown md(ptask);
        auto vimcmd = "vim " + md.path();
        system(vimcmd.c_str());
    }

    // TODO: error handle
    // std cmd: new date epic title
    void create(std::vector<std::string>& cmd_tokens) override {
        CHECK(cmd_tokens.size() > 1) << "not enough cmd";
        if (!util::is_date(cmd_tokens[1])) {
            cmd_tokens.insert(cmd_tokens.begin()+1, "today");
        }
        parse_cmd(cmd_tokens);

        if (taskbook_.epic_exist(epic_)) {
            create_task();
        } else {
            // TODO: enable epics with spaces in between
            std::cout << "Epic [" + epic_
                + "] does not exist, the similar one could be: " << std::endl;
            // TODO: find similar ones, med
            std::cout << "would you like to create one? [y/n]" << std::endl;
            std::cout << ">> ";
            std::string response;

            while (std::cin >> response) {
                if (response == "y") {
                    std::vector<std::string> epic_cmd({"enew", epic_});
                    EpicHandler tmp(calendar_, taskbook_);
                    tmp.create(epic_cmd);
                    create_task();
                    break;
                } else if (response == "n") {
                    std::cout << "end process" << std::endl;
                    return ;
                } else {
                    std::cout << "please enter [y/n]" << std::endl;
                }
            } // end  while
        } // end if epic not exist
    }

    void print(std::vector<std::string>& cmd) override {
        if (cmd.size() == 1) {
            // ls
            InfoTask info(calendar_, DateW::today());
        } else if (cmd.size() == 2) {
            // ls date
            auto date = DateW::to_date(cmd[1]);
            InfoTask info(calendar_, date);
        } else {
            LOG(INFO) << "error cmd";
        }
    }

    void print_arch(cmdvec& cmd) {
        InfoArch p(calendar_);
    }

    void print_duration(std::vector<std::string>& cmd) {
        // ls week
        auto cur_date = DateW::today();
        for (int i = 0; i < 7; i++) {
            if (i) {
                std::cout << std::endl;
            }
            cmd[1] = cur_date.to_month_day();
            print(cmd);
            cur_date = cur_date + 1;
        }
    }

    void repeat(std::vector<std::string>& cmd) {
        // parser_.accept(cmd);
        // parse cmd
        // TODO: need a real parser
        // parser_.regist("rp", 0);
        // parser_.regist("id", 1);
        // parser_.regist("date", -1);

        auto ptask = taskbook_.get_task_by_id(cmd[1]);
        for (size_t i = 2; i < cmd.size(); ++i) {
            std::vector<std::string> new_cmd({"new", 
            cmd[i], ptask->epic(), ptask->title()});
            create(new_cmd);
        }
    }

    void tick(std::vector<std::string>& cmd) {
        // tick id
        auto ptask = taskbook_.get_task_by_id(cmd[1]);
        CHECK(ptask->status() == VALUE::todo);
        ptask->set_status(VALUE::done);
        InfoTask info(calendar_, DateW::today());
    }

private:
    void create_task() {
        auto id = taskbook_.get_id();
        TaskPtr ptask(new Task(
                    id, DateW::to_date(time_), epic_, text_));
        taskbook_.add_task(ptask);
        calendar_.add_task(ptask);
    }

    // TODO: reverse cmd to make it faster
    void parse_epic(std::vector<std::string>& cmd) {
        epic_.clear();
        if (util::is_number(cmd[0])) {
            epic_ = taskbook_.get_epic_by_id(std::stoi(cmd[0]));
            cmd.erase(cmd.begin());
            return;
        } else if(cmd[0].front() != '/') {
            epic_ = cmd[0];
            cmd.erase(cmd.begin());
            return;
        } else {
            epic_ = cmd[0].substr(1, cmd[0].size()-1);
            int cnt = 0;
            while(cnt < cmd.size()) {
                if(cnt) epic_ += " "  + cmd[cnt];
                if (cmd[cnt].back() == '/') break;
                cnt ++;
            }

            CHECK(cnt < cmd.size());
            epic_.pop_back();
            cmd.erase(cmd.begin(), cmd.begin()+cnt+1);
        }
    }

    void parse_cmd(std::vector<std::string>& cmd) {
        head_ = cmd[0];
        cmd.erase(cmd.begin());
        time_ = cmd[0];
        cmd.erase(cmd.begin());
        parse_epic(cmd);
        text_ = util::join(cmd);
    }
};

} // namespace tomato 
#endif // TOMATO_CMD_HANDLER_H
