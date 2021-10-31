#ifndef TOMATO_TASK_H
#define TOMATO_TASK_H
#include <string>
#include <iostream>
#include "pugixml.hpp"
#include "template.h"
namespace tomato {

class Task {
private:
    pugi::xml_node root_;
public:
    Task(std::string epic, std::string discription) {
        root_ = xml::load_xml(TEMPLATE::task);

        xml::set_text(root, "epic", epic);
        xml::set_text(root, "discription", discription);
    }
};

 
} // namespace tomato 

#endif // TOMATO_TASK_H