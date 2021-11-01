#include <iostream>
#include <string>
#include "inc/cmd.h"
#include "inc/calendar.h"
int main() {
    tomato::TaskBook taskbook;
    tomato::Calendar calendar;
    tomato::CMD cmd_reader(calendar, taskbook);
    std::string cmd;
    std::cout << ">> ";
    while (std::getline(std::cin, cmd)) {
        if (cmd.empty()) continue;
        if (cmd == "quit") {
            break;
        } else {
            cmd_reader.execute(cmd);
        } 
    }
}