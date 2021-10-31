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
public:
    TaskBook() {
        pugi::xml_parse_result result 
            = doc_.load_file(TEMPLATE::taskbook.c_str());
        std::string status = result.description();
        assert(status == "No error");
        std::cout << result.description() << std::endl;
    }

    ~TaskBook() {
        doc_.save_file(TEMPLATE::taskbook.c_str());
    }

    void add_task(Task& task) {
        task.node().print(std::cout);
        doc_.child("root").append_copy(task.node());
        doc_.child("root").print(std::cout);
    }

};

} // namespace tomat 

#endif // TOMATO_TASKBOOK_H