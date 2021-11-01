#ifndef TOMATO_XMLPARSER_H
#define TOMATO_XMLPARSER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <unordered_map>
#include "inc/template.h"
#include "inc/util.h"
namespace tomato {
namespace naive_xml {
class Node;
using NodePtr = std::shared_ptr<Node>;
// super naive parser
class Node {
private:
    std::string label_;
    std::string text_;
public:
    Node(std::string token) {
        CHECK(token.size() >=3) << "token size too small";
        label_ = token.substr(1, token.size()-2);
    }

    Node(std::string token, std::string text) {
        CHECK(token.size() >=3);
        label_ = token.substr(1, token.size()-2);
        text_ = text;
    }

    void set_text(std::string text) {
        text_ = text;
    }

    inline const std::string& text() {
        return text_;
    }

    inline const std::string& label() { return label_; }

    std::string start_token() {
        return "<" + label_ + ">";
    }
    
    std::string end_token() {
        return "</" + label_ + ">";
    }

    inline void append_node(NodePtr node) {
        kids.push_back(node);
    }

    std::vector<NodePtr> kids;
};

void print_node(std::ostream& fout, NodePtr node, int indent=0) {
    int cnt = indent;
    while(cnt-->0) {
        fout <<" ";
    }

    fout << node->start_token();
    if (node->text().empty()) {
        fout << std::endl;
        for(auto kid : node->kids) {
            print_node(fout, kid, indent+4);
        }
        cnt = indent;
        while(cnt-->0) {
            fout <<" ";
        }
    } else {
        CHECK(node->kids.empty());
        fout << node->text();
    }
    fout << node->end_token() << std::endl;
}

class Parser {
private:
    NodePtr root_;
    std::string text_;
    std::unordered_map<std::string, std::vector<NodePtr>> label_index_;
    std::unordered_map<std::string, std::vector<NodePtr>> id_index_;

public:
    NodePtr load_path(std::string path) {
        std::ifstream fin(path);
        std::string line;
        while(std::getline(fin, line)) {
            text_ += line;
        }
        fin.close();
        auto ret = dfs_read(0);
        build_index(ret.ptr);
        root_ = ret.ptr;
        return root_;
    }

    inline NodePtr get_root() { return root_;}

    NodePtr first_node(std::string label) {
        CHECK(label_index_.count(label) != 0);
        return label_index_.at(label)[0];
    }

    inline void print(std::ostream& fout) {
        print_node(fout, root_);
    }

private:
    void insert_index(std::unordered_map<std::string, std::vector<NodePtr>>& index, 
        std::string target, NodePtr node) {
        if (index.count(target) == 0) {
            index[target] = std::vector<NodePtr>{node};
        } else {
            index.at(target).push_back(node);
        }
    }

    void build_index(NodePtr root) {
        insert_index(label_index_, root->label(), root);
        if (root->label() == TAG::id) {
            insert_index(id_index_, root->label(), root);
        }
        for (auto& kid : root->kids) {
            build_index(kid);
        }
    }
    size_t skip_space(size_t pos) {
        while (pos < text_.size()) {
            if (text_[pos] != ' ' && text_[pos] != '\n' && text_[pos] != '\t')  
                return pos;
            pos ++;
        }
        return pos;
    }
    bool is_end_token(size_t pos) {
        return text_[pos] == '<' && text_[pos+1] == '/';
    }

    bool is_start_token(size_t pos) {
        return text_[pos] == '<' && text_[pos+1] != '/';
    }

    bool token_match(std::string start, std::string end) {
        if (start.size() + 1 != end.size()) return false;
        for (size_t i = 1; i < start.size(); ++i) {
            if (start[i] != end[i+1]) return false;
        }
        return true;
    }

    struct DfsSig {
        size_t pos;
        std::string text;
        bool is_text;
        NodePtr ptr;
        DfsSig(std::string t, size_t p, bool is_t=false) {
            text = t;
            pos = p;
            is_text = is_t;
        }
        
        DfsSig(std::string t, size_t p, NodePtr node_ptr) {
            text = t;
            pos = p;
            is_text = false;
            ptr = node_ptr; 
        }
    };

    DfsSig dfs_read(size_t pos) {
        pos = skip_space(pos);
        if (pos >= text_.size()) {
            CHECK(false) << "why we're here";
        }

        if (is_start_token(pos)) {
            std::string token;
            auto nxt_pos = get_token(pos, token)+1;
            NodePtr nodeptr(new Node(token));
            while(true) {
                auto sig = dfs_read(nxt_pos);
                nxt_pos = sig.pos;
                if (token_match(token, sig.text)) {
                    break;
                }

                if (sig.is_text) {
                    nodeptr->set_text(sig.text);
                } else {
                    CHECK(sig.ptr != nullptr) ;
                    nodeptr->kids.push_back(sig.ptr);
                }
            }
            return DfsSig(token, nxt_pos, nodeptr);

        } else if (is_end_token(pos)) {
            std::string token;
            auto nxt_start = get_token(pos, token)+1;
            return DfsSig(token, nxt_start);

        } else {
            // text
            std::string text;
            auto next_start = get_token(pos, text, '<');
            text.pop_back();
            return DfsSig(text, next_start, true);
        }
    }

    size_t get_token(size_t pos, std::string& ret, char ed='>') {
        for(; pos < text_.size(); ++pos) {
            ret += text_[pos];
            if (text_[pos] == ed) {
                break;
            }
        }
        return pos;
    }
};

} // end namespace xml
} // end namespace xml
#endif

