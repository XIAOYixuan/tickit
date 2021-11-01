#ifndef TOMATO_VIEW_TABLE_PRINTER_H
#define TOMATO_VIEW_TABLE_PRINTER_H
#include <iostream>
#include <string>
#include <vector>

#include "inc/util.h"
#include "inc/template.h"
#include "inc/task.h"
#include "tabulate/table.hpp"

namespace tomato{
namespace view{
class TablePrinter {
protected:
    tabulate::Table table_;
public:
    TablePrinter() = default;
    friend std::ostream& operator<<(std::ostream& os, const TablePrinter& table);
};
std::ostream& operator<<(std::ostream& os, const TablePrinter& tb) {
    os << tb.table_;
    return os;
}
class TaskTable : public TablePrinter {
    
public:
    void print_info(std::vector<TaskPtr>& ptasks) {
        table_.add_row({TAG::id, TAG::title, TAG::start, TAG::end});
        for(auto& ptask: ptasks) {
            table_.add_row(
                {std::to_string(ptask->id()), ptask->title(), ptask->start(), ptask->end()});
        }
        table_[0].format().font_style({tabulate::FontStyle::bold});
        std::cout << table_ << std::endl;
    }
};
    
} // namespace view
} // namespace tomato
#endif // TOMATO_VIEW_TABLE_PRINTER_H