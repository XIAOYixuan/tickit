#ifndef TOMATO_INFO_PRINTER_H
#define TOMATO_INFO_PRINTER_H
#include "inc/util.h"
#include "inc/calendar.h"
#include "inc/date/date_wrapper.h"
#include "inc/view/table_printer.h"
namespace tomato {
class InfoPrinter {
private:
    Calendar& calendar_;
public:

    InfoPrinter(Calendar& calendar, DateW& dt) 
        : calendar_(calendar) {
        list_info(dt);
    }

private:
    void list_info(DateW& dt) {
        std::cout << dt << std::endl;
        auto tasks = calendar_.get_tasks(dt);
        view::TaskTable task_printer;
        task_printer.print_info(tasks);
    };
};

} // namespace tomato

#endif // TOMATO_INFO_PRINTER_H