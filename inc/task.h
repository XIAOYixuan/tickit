#ifndef TOMATO_TASK_H
#define TOMATO_TASK_H
#include <string>
#include <memory>
#include <iostream>
#include "inc/template.h"
#include "inc/xml_wrapper.h"
#include "inc/date/date_wrapper.h"
#include "inc/util.h"
namespace tomato {

class Task {
protected:
    std::shared_ptr<xml::Doc> pdoc_; // load from xml path
    std::shared_ptr<xml::Node> proot_; // load from memory/task book
    int id_; 
    std::string title_, epic_, description_, status_;
    std::string start_time_, end_time_;
    std::string label_;
    std::string attach_;
    DateW date_;
public:
    Task() = default;
    Task(xml::Node& task_node) {
        proot_.reset(new xml::Node(task_node.ptr())); 
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

    Task(int id, DateW dt, std::string epic, std::string title){
        pdoc_.reset(new xml::Doc());
        xml::Doc& root_ = *pdoc_;
        root_.load_path(TEMPLATE::task);

        id_ = id;
        root_.set_text(TAG::id, std::to_string(id_));
        date_ = dt;
        root_.set_text(TAG::date, date_.to_string());        
        epic_ = epic;
        root_.set_text(TAG::epic, epic);
        title_ = title;
        root_.set_text(TAG::title, title);
        start_time_ = "---";
        root_.set_text(TAG::start, start_time_);
        end_time_ = "---";
        root_.set_text(TAG::end, end_time_);
    }

    xml::Node& node() { 
        CHECK(pdoc_ != nullptr);
        return pdoc_->get_root(); 
    }

    void set_status(std::string st) {
        if (pdoc_) {
            pdoc_->set_text(TAG::status, st);
        } else {
            CHECK(proot_);
            proot_->set_kid_text(TAG::status, st);
        }
        status_ = st;
    }
    
    inline DateW& date() { return date_; }
    inline int& id() { return id_;}
    // TODO: dangerous
    inline std::string& title() { return title_; }
    inline std::string& start() { return start_time_; }
    inline std::string& end() { return end_time_; }
    inline std::string& epic() { return epic_; }
    inline std::string& status() { return status_; }

};

using TaskPtr = std::shared_ptr<Task>;

class Epic : public Task {
public:
    Epic(xml::Node& epic_node) : Task(epic_node) {}
    Epic(int id, DateW& dt, std::string title) {
        pdoc_.reset(new xml::Doc());
        auto& root_ = *pdoc_;
        root_.load_path(TEMPLATE::epic);
        id_ = id;
        root_.set_text(TAG::id, std::to_string(id_));
        title_ = title;
        root_.set_text(TAG::title, title);
    }
};
using EpicPtr = std::shared_ptr<Epic>;
 
} // namespace tomato 

#endif // TOMATO_TASK_H