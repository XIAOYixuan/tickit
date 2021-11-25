#ifndef TOMATO_TASKBOOK_H
#define TOMATO_TASKBOOK_H
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "inc/util.h"
#include "inc/template.h"
#include "inc/task.h"
#include "inc/xml_wrapper.h"
namespace tomato {

class TaskBook {
    xml::Doc doc_;
    std::vector<TaskPtr> tasks_;
    std::unordered_map<int, TaskPtr> id_tasks_;
    // maybe use epic id?
    std::unordered_map<std::string, EpicPtr> epics_;
    std::unordered_map<std::string, EpicPtr> arch_epics_;
public:
    TaskBook() {
        doc_.load_path(TEMPLATE::taskbook);
        load_book();
    }

    ~TaskBook() {
        save();
    }

    void save() {
        doc_.save_file(TEMPLATE::taskbook);
    }

    // TODO: make it faster
    std::string get_epic_by_id(int id) {
        for (auto& it : epics_) {
            if (it.second->id() == id) {
                return it.first;
            }
        }
        CHECK(false) << "can't find epics with id [" << id << "]";
        return "";
    }

    int get_id() {
        std::string id_str = doc_.get_node(TAG::id).value();
        auto id = std::stoi(id_str);
        auto new_id = id+1;
        doc_.set_text(TAG::id, new_id);
        return id;
    }

    int get_epic_id() {
        std::string id_str = doc_.get_node(TAG::epic_id).value();
        auto id = std::stoi(id_str);
        auto new_id = id+1;
        doc_.set_text(TAG::epic_id, new_id);
        return id;
    }

    void add_task(TaskPtr ptask) {
        // ptask->node().print();
        // DLOG(INFO) << "-------------------";
        // doc_.print();
        doc_.append_node(TAG::tasks, ptask->node());
        // DLOG(INFO) << "-------------------";
        // doc_.print();
        tasks_.push_back(ptask);
        id_tasks_[ptask->id()] = ptask;
    }
    
    inline TaskPtr get_task_by_id(int id) {
        return id_tasks_.at(id);
    }

    inline TaskPtr get_task_by_id(std::string id) {
        return get_task_by_id(std::stoi(id));
    }

    void add_epic(EpicPtr pepic) {
        CHECK(epics_.count(pepic->title()) == 0);
        DLOG(INFO) << pepic->node() << std::endl;
        doc_.append_node(TAG::epics, pepic->node());
        epics_[pepic->title()] = pepic;
    }

    void archiv_epic(int id) {
        auto title = get_epic_by_id(id);
        auto pepic = epics_.at(title);
        CHECK(epics_.count(pepic->title()) != 0);
        pepic->set_status(VALUE::arch);
        epics_.erase(pepic->title());
        arch_epics_[pepic->title()] = pepic;
    }

    inline bool epic_exist(std::string title) {
        return epics_.count(title) != 0;
    }

    inline bool is_arch(std::string title) {
        return arch_epics_.count(title) != 0;
    }
    inline std::vector<TaskPtr>& tasks() { return tasks_; }
    inline std::unordered_map<std::string, EpicPtr>& epics() { return epics_;}

private:
    void load_book() {
        // TODO: very problematic here...
        auto task_nodes = doc_.get_node(TAG::tasks);
        // task
        for(size_t i = 0; i < task_nodes.size(); ++i) {
            auto task_node = task_nodes.get_item(i);
            auto ptask = TaskPtr(new Task(task_node));
            tasks_.push_back(ptask);
            id_tasks_[ptask->id()] = ptask;
        }

        auto epic_nodes = doc_.get_node(TAG::epics);
        for(size_t i = 0; i < epic_nodes.size(); ++i) {
            auto epic_node = epic_nodes.get_item(i);
            auto epic_name = epic_node.get_kid_value("title");
            auto pepic = EpicPtr(new Epic(epic_node));
            if (!epic_exist(epic_name) && pepic->status() == VALUE::todo) {
                epics_[epic_name] = pepic; 
            } else if (pepic->status() == VALUE::arch 
                && arch_epics_.count(epic_name) == 0) {
                arch_epics_[epic_name] = pepic;
            }
        }
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H
