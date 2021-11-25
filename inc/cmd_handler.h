#ifndef TOMATO_CMD_HANDLER_H
#define TOMATO_CMD_HANDLER_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
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
using cmdvec = std::vector<std::string>;
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

    // for spaced repetition
    // TODO: better methods, less operation, e.g. store the latest update
    void repe(cmdvec& cmd) {
        if (cmd.size() == 1) {
            cmd.push_back("2");
        }
        CHECK(util::is_number(cmd[1])) << "repe days";
        InfoRepe(taskbook_, std::stoi(cmd[1]));
    }

};

class TaskHandler : public BaseCMDHandler {
    std::string head_, time_, epic_, text_;
using cmdvec = std::vector<std::string>;
private:
    // cmd id
    TaskPtr check_cmd_id(cmdvec& cmd) {
        if (cmd.size() < 2 || !util::is_number(cmd[1])) {
            LOG(INFO) << "invalid cmd : " << util::join(cmd);
        }
        auto ptask = taskbook_.get_task_by_id(cmd[1]);
        if (ptask == nullptr) {
            LOG(INFO) << "unkwnon id : " << cmd[1];
        }
        return ptask;
    }
public:
    TaskHandler(Calendar& c, TaskBook& t) : BaseCMDHandler(c, t) {}
    
    void set_time(cmdvec& cmd) {
        auto ptask = check_cmd_id(cmd);
        ptask->set_start(cmd[2]);
    }

    void pomodora(cmdvec& cmd) {
        // timer and reminder
        auto ptask = check_cmd_id(cmd);
        auto st_time = util::get_cur_time();
        auto pclock = "python " + TEMPLATE::clock_prog;
        system(pclock.c_str());
        auto ed_time = util::get_cur_time();
        ptask->add_timestamp(st_time, ed_time);
    }

    void move(cmdvec& cmd) {
        if (cmd.size() < 3) {
            LOG(INFO) << "need >three arguments, move id date";
        }
        if (!util::is_date(cmd.back())) {
            LOG(INFO) << cmd.back() << " is not a date";
        }
        auto date = DateW::to_date(cmd.back());
        for (size_t i = 1; i + 1 < cmd.size(); ++i) {
            cmd[1] = cmd[i];
            auto ptask = check_cmd_id(cmd);
            calendar_.move_date(ptask, date);
        }
    }

    void rm(cmdvec& cmd) {
        for (size_t i = 1; i < cmd.size(); ++i) {
            cmd[1] = cmd[i];
            auto ptask = check_cmd_id(cmd);
            if (ptask->status() == VALUE::arch) {
                calendar_.aktiv_task(ptask);
                ptask->set_status(VALUE::todo);
            }
            calendar_.delete_task(ptask);
            ptask->set_status(VALUE::dele);
        }
    }

    void drop(cmdvec& cmd) {
        auto ptask = check_cmd_id(cmd);
        if (ptask == nullptr) return;
        calendar_.drop_task(ptask);
        ptask->set_status(VALUE::arch);
    }

    void aktiv(cmdvec& cmd) {
        auto ptask = check_cmd_id(cmd);
        if (ptask == nullptr) return;
        calendar_.aktiv_task(ptask);
        ptask->set_status(VALUE::todo);
    }

    void edit(std::vector<std::string>& cmd) {
        auto id = std::stoi(cmd[1]);
        auto ptask = taskbook_.get_task_by_id(id);
        TaskMarkDown md(ptask);
        auto vimcmd = "nvim " + md.path();
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
            InfoTask info(calendar_, taskbook_.epics(), DateW::today());
        } else if (cmd.size() == 2) {
            // ls date
            auto date = DateW::to_date(cmd[1]);
            InfoTask info(calendar_, taskbook_.epics(), date);
        } else {
            LOG(INFO) << "error cmd";
        }
    }

    void print_arch(cmdvec& cmd) {
        InfoArch p(calendar_, taskbook_.epics());
    }
    
    DateW locate_monday(DateW cur_date) {
        // locate monday
        for (int i = 0; i < 7; ++i)  {
            cur_date = cur_date - 1;
            // TODO: don't use string here
            if (cur_date.to_weekday() == "Mon") {
                return cur_date;
            }
        }
        CHECK(false) << "monday 404";
        return cur_date;
    }

    std::vector<DateW> gather_days(DateW start, int days) {
        std::vector<DateW> ret;
        for (int i = 0; i < days; ++i) {
            ret.push_back(start);
            start = start + 1;
        }
        return ret;
    }

    void stat_week(cmdvec& cmd) {
        DateW cur_date;
        if (cmd.size() == 1) {
            cur_date = DateW::today();
        } else {
            cur_date = DateW::to_date(cmd[1]);
        }
        cur_date = locate_monday(cur_date);
        auto days = gather_days(cur_date, 7);
        StatTask(calendar_, taskbook_.epics(), days) ;
        // group_by_epic();
        // print_result();
    }


    void print_this_week(cmdvec& cmd) {
        auto cur_date = DateW::today();
        if (cmd.size() == 3) {
            CHECK(cmd[2] == "nx");
            cur_date = cur_date + 7;
            // next week
        }
        cur_date = locate_monday(cur_date);
        cmd.pop_back();
        print_duration(cur_date, 7, cmd);
    }

    void print_duration(DateW& cur_date, int days, cmdvec& cmd) {
        auto alldays = gather_days(cur_date, days);
        // ls this week
        for (int i = 0; i < days; i++) {
            if (i) {
                std::cout << std::endl;
            }
            cmd[1] = alldays[i].to_month_day();
            print(cmd);
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
        InfoTask info(calendar_, taskbook_.epics(), DateW::today());
    }

private:
    void create_task() {
        auto id = taskbook_.get_id();
        TaskPtr ptask(new Task(
                    id, DateW::to_date(time_), epic_, text_));
        taskbook_.add_task(ptask);
        calendar_.add_task(ptask);
        std::cout << "created " << ptask->id() << " " << ptask->title() << std::endl;
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
