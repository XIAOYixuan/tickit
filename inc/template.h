#ifndef TOMATO_TEMPLATE_H
#define TOMATO_TEMPLATE_H
#include <string>
namespace tomato {
class TEMPLATE {
public:
    static std::string task; 
};
std::string TEMPLATE::task = "templates/today.task.xml";
} // end namespace tomato
#endif // TOMATO_TEMPLATE_H