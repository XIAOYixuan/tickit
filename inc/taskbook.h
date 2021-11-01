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
public:
    TaskBook() {
        doc_.load_path(TEMPLATE::taskbook);
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

    void add_task(Task& task) {
        task.node().print();
        DLOG(INFO) << "-------------------";
        doc_.print();
        doc_.append_node("tasks", task.node());
        DLOG(INFO) << "-------------------";
        doc_.print();
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H