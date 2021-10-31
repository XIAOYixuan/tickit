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
    int id_; 
public:
    Task(int id, std::string epic, std::string description) {
        id_ = id;
        root_ = xml::load_xml(TEMPLATE::task);
        xml::set_text(root_, TAG::epic, epic);
        // root_.print(std::cout);
        xml::set_text(root_, TAG::description, description);
        // root_.print(std::cout);
        xml::set_text(root_, TAG::id, id_);
        root_.print(std::cout);
    }

    pugi::xml_node& node() { return root_; }
};
 
} // namespace tomato 

#endif // TOMATO_TASK_H