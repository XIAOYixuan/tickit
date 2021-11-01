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
    std::unordered_map<std::string, EpicPtr> epics_;
public:
    TaskBook() {
        doc_.load_path(TEMPLATE::taskbook);
        load_book();
    }

    ~TaskBook() {
        doc_.save_file(TEMPLATE::taskbook);
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
    }

    void add_epic(EpicPtr pepic) {
        CHECK(epics_.count(pepic->title()) == 0);
        DLOG(INFO) << pepic->node() << std::endl;
        doc_.append_node(TAG::epics, pepic->node());
        epics_[pepic->title()] = pepic;
    }

    inline bool epic_exist(std::string title) {
        return epics_.count(title) != 0;
    }

    inline std::vector<TaskPtr>& tasks() { return tasks_; }
    inline std::unordered_map<std::string, EpicPtr>& epics() { return epics_;}

private:
    void load_book() {
        auto task_nodes = doc_.get_node(TAG::tasks);
        for(size_t i = 0; i < task_nodes.size(); ++i) {
            auto task_node = task_nodes.get_item(i);
            tasks_.push_back(TaskPtr(new Task(task_node)));
        }

        auto epic_nodes = doc_.get_node(TAG::epics);
        for(size_t i = 0; i < epic_nodes.size(); ++i) {
            auto epic_node = epic_nodes.get_item(i);
            if (!epic_exist(epic_node.label())) {
                epics_[epic_node.label()] = EpicPtr(new Epic(epic_node));
            }
            
        }
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H