#ifndef TOMATO_XML_WRAPPER_H
#define TOMATO_XML_WRAPPER_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
//#include "rapidxml/rapidxml_ext.h"
#include "xmlparser/xmlparser.h"
namespace tomato{
namespace xml{
class Node {
private:
    naive_xml::NodePtr pnode_;
public:
    Node(naive_xml::NodePtr node) {
        pnode_ = node;
    }

    naive_xml::NodePtr ptr() {
        return pnode_;
    }

    void print() {
        naive_xml::print_node(pnode_);
    }

    std::string value() {
        return pnode_->text();
    }
};

class Doc {
private:
    naive_xml::Parser parser_; 
public:
    Doc() {}
    Doc(std::string path) {
        load_path(path);
    }
    ~Doc() {
    }
    
    void load_path(std::string path) {
        parser_.load_path(path);
    }

    Node get_root() {
        return Node(parser_.get_root());
    }

    Node get_node(std::string name) {
        // print();
        auto pnode = parser_.first_node(name);
        Node ret(pnode);
        return ret;
    }
    
    void append_node(std::string dest, Node node) {
        auto dest_node = get_node(dest).ptr();
        // TODO: shallow copy?
        dest_node->append_node(node.ptr());
    }

    void set_text(std::string dest, std::string text) {
        auto dest_node = get_node(dest).ptr();
        dest_node->set_text(text);
    }

    void set_text(std::string dest, int text) {
        set_text(dest, std::to_string(text));
    }

    void print() {
        parser_.print();
    }

    void save_file(std::string path) {
    }
};


} // namespace xml
} // namespace tomato

#endif // TOMATO_XML_WRAPPER_H