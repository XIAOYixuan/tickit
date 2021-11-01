#ifndef TOMATO_TASK_H
#define TOMATO_TASK_H
#include <string>
#include <iostream>
#include "template.h"
#include "xml_wrapper.h"
namespace tomato {

class Task {
private:
    xml::Doc root_;
    int id_; 
public:
    Task(int id, std::string epic, std::string title){
        id_ = id;
        root_.load_path(TEMPLATE::task);
        root_.set_text(TAG::epic, epic);
        // root_.print(std::cout);
        //xml::set_text(root_, TAG::title, title);
        root_.set_text(TAG::title, title);
        // root_.print(std::cout);
        root_.set_text(TAG::id, std::to_string(id_));
    }

    xml::Node node() { return root_.get_root(); }
};
 
} // namespace tomato 

#endif // TOMATO_TASK_H