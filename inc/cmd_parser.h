#include <string>
#include <vector>
#include <unordered_map>
#include "inc/util.h"

namespace tomato {

class CMDParser {
private:
    std::vector<std::string> argc_;
    std::vector<std::string> cmd_;
    std::unordered_map<std::string, int> argc_conf_;
public:
    CMDParser() = default;

    void accept(std::vector<std::string>& cmd) {
        cmd_ = cmd;
        argc_.clear();
        argc_conf_.clear();
    }

    void regist(std::string name, int num) {
        argc_.push_back(name);
        argc_conf_[name] = num;
    }
};
    
} // namespace tomato 