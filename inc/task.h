#ifndef TOMATO_TASK_H
#define TOMATO_TASK_H
#include <string>
#include <memory>
#include <iostream>
#include "inc/template.h"
#include "inc/xml_wrapper.h"
#include "inc/date/date_wrapper.h"
namespace tomato {

class Task {
private:
    xml::Doc root_;
    int id_; 
    std::string title_, epic_, description_, status_;
    std::string start_time_, end_time_;
    std::string label_;
    std::string attach_;
    DateW date_;
public:
    Task(xml::Node& task_node) {
        for(size_t i = 0; i < task_node.size(); ++i) {
            auto tag = task_node.get_item(i);
            auto label = tag.label();
            auto text = tag.value();

            if (label == TAG::id) {
                id_ = std::stoi(text);
            } else if (label== TAG::title) {
                title_ = text;
            } else if (label == TAG::description) {
                description_ = text;
            } else if (label == TAG::epic) {
                epic_ = text;
            } else if (label == TAG::status) {
                status_ = text;
            } else if (label == TAG::label) {
                label_ = text;
            } else if (label == TAG::start) {
                start_time_ = text;
            } else if (label == TAG::end) {
                end_time_ = text;
            } else if (label == TAG::attachment) {
                attach_ = text;
            } else if (label == TAG::date) {
                date_ = DateW(text);
            } else  {
                CHECK(false) << "unseen attr [" << label << " " << text << "]";
            }
        }
    }

    Task(int id, DateW& dt, std::string epic, std::string title){
        id_ = id;
        root_.load_path(TEMPLATE::task);
        date_ = dt;
        root_.set_text(TAG::epic, epic);
        epic_ = epic;
        root_.set_text(TAG::title, title);
        title_ = title;
        root_.set_text(TAG::id, std::to_string(id_));

        start_time_ = "---";
        end_time_ = "---";
    }

    xml::Node node() { return root_.get_root(); }

    inline DateW& date() { return date_; }
    inline int& id() { return id_;}
    inline std::string& title() { return title_; }
    inline std::string& start() { return start_time_; }
    inline std::string& end() { return end_time_; }

};

using TaskPtr = std::shared_ptr<Task>;
 
} // namespace tomato 

#endif // TOMATO_TASK_H