#ifndef TOMATO_XML_WRAPPER_H
#define TOMATO_XML_WRAPPER_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include "inc/util.h"
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

    inline void print(std::ostream& fout) {
        naive_xml::print_node(fout, pnode_);
    }
    
    inline void print() {
        print(std::cout);
    }

    inline std::string value() {
        return pnode_->text();
    }

    inline std::string label() {
        return pnode_->label();
    }

    void set_kid_text(std::string nlabel, std::string ntext) {
        for (auto& kid : pnode_->kids) {
            if (kid->label() == nlabel) {
                kid->set_text(ntext);
                return;
            }
        }
        CHECK(false) << "label [" << nlabel << "] not found in <"
            << label() << ">" << value() << "</" << label() << ">";
    }

    Node get_item(size_t pt) {
        auto raw = pnode_->kids[pt];
        return Node(raw);
    }
    
    Node get_kid(std::string tag) {
        for (auto kid : pnode_->kids) {
            if (kid->label() == tag) {
                return Node(kid);
            }
        }
        CHECK(false) << "tag [" << tag << "] not found";
        return Node(nullptr);
    }

    std::string get_kid_value(std::string tag) {
        for (auto kid : pnode_->kids) {
            if (kid->label() == tag) {
                return kid->text();
            }
        }
        CHECK(false) << "tag [" << tag << "] not found";
        return "";
    }

    size_t size() {
        return pnode_->kids.size();
    }

    friend std::ostream& operator<<(std::ostream& os, Node& node);
};
std::ostream& operator<<(std::ostream& os, Node& node) {
    node.print(os);
    return os;
}

class Doc {
private:
    naive_xml::Parser parser_; 
    std::shared_ptr<Node> proot_;
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

    Node& get_root() {
        if (proot_ == nullptr) {
            proot_.reset(new Node(parser_.get_root()));
        }
        return *proot_;
    }

    Node get_node(std::string name) {
        // print();
        auto pnode = parser_.first_node(name);
        CHECK(pnode);
        return Node(pnode);
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

    void print(std::ostream& fout) {
        parser_.print(fout);
    }

    void print() {
        parser_.print(std::cout);
    }

    void save_file(std::string path) {
        std::ofstream fout;
        fout.open(path);
        print(fout);
        fout.close();
    }
    friend std::ostream& operator<<(std::ostream& os, Doc& doc);

};
std::ostream& operator<<(std::ostream& os, Doc& doc) {
    doc.print(os);
    return os;
}


} // namespace xml
} // namespace tomato

#endif // TOMATO_XML_WRAPPER_H