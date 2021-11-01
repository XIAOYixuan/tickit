#ifndef TOMATO_INFO_PRINTER_H
#define TOMATO_INFO_PRINTER_H
#include "inc/util.h"
#include "inc/calendar.h"
#include "inc/date/date_wrapper.h"
namespace tomato {
class InfoPrinter {
private:
    Calendar& calendar_;
public:

    InfoPrinter(Calendar& calendar, std::vector<std::string> & cmd) 
        : calendar_(calendar) {
        if(cmd.size() == 1) {
            list_info(DateW());
        }
    }

private:
    void list_info(DateW dt) {
        std::cout << dt << std::endl;
    };
};

} // namespace tomato

#endif // TOMATO_INFO_PRINTER_H