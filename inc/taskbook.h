#ifndef TOMATO_TASKBOOK_H
#define TOMATO_TASKBOOK_H
#include <iostream>
#include <vector>
#include <string>
#include "inc/util.h"
#include "inc/template.h"
#include "inc/task.h"
#include "inc/xml_wrapper.h"
namespace tomato {

class TaskBook {
    xml::Doc doc_;
    std::vector<TaskPtr> tasks_;

public:
    TaskBook() {
        doc_.load_path(TEMPLATE::taskbook);
        load_book();
    }

    ~TaskBook() {
        doc_.save_file(TEMPLATE::taskbook);
    }

    int get_id() {
        std::string id_str = doc_.get_node("id").value();
        auto id = std::stoi(id_str);
        auto new_id = id+1;
        doc_.set_text(TAG::id, new_id);
        return id;
    }

    void add_task(TaskPtr ptask) {
        ptask->node().print();
        DLOG(INFO) << "-------------------";
        doc_.print();
        doc_.append_node("tasks", ptask->node());
        DLOG(INFO) << "-------------------";
        doc_.print();
        tasks_.push_back(ptask);
    }

    inline std::vector<TaskPtr>& tasks() { return tasks_; }

private:
    void load_book() {
        auto task_nodes = doc_.get_node(TAG::tasks);
        for(size_t i = 0; i < task_nodes.size(); ++i) {
            auto task_node = task_nodes.get_item(i);
            tasks_.push_back(TaskPtr(new Task(task_node)));
        }
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H