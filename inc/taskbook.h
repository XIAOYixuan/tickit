#ifndef TOMATO_TASKBOOK_H
#define TOMATO_TASKBOOK_H
#include <iostream>
#include <vector>
#include <string>
#include "inc/util.h"
#include "inc/template.h"
#include "inc/task.h"
#include "pugixml.hpp"
namespace tomato {

class TaskBook {
    pugi::xml_document doc_;
    pugi::xml_node id_;
    pugi::xml_node tasks_;
public:
    TaskBook() {
        pugi::xml_parse_result result 
            = doc_.load_file(TEMPLATE::taskbook.c_str());
        std::string status = result.description();
        assert(status == "No error");
        std::cout << result.description() << std::endl;
        id_ = doc_.child("root").child("id");
        tasks_ = doc_.child("root").child("tasks");
    }

    ~TaskBook() {
        doc_.save_file(TEMPLATE::taskbook.c_str());
    }

    int get_id() {
        auto id = id_.text().as_int(); 
        auto new_id = id+1;
        xml::set_text(id_, std::to_string(new_id));
        id_.print(std::cout);
        return id;
    }

    void add_task(Task& task) {
        task.node().print(std::cout);
        tasks_.append_copy(task.node());
        tasks_.print(std::cout);
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H