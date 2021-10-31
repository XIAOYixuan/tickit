#include <iostream>
#include <string>
#include "inc/cmd.h"
#include "inc/calendar.h"
int main() {
    tomato::Calendar calendar;
    tomato::CMD cmd_reader(calendar);
    std::string cmd;
    std::cout << ">> ";
    while (std::cin >> cmd) {
        if (cmd == "quit") {
            break;
        } else {
            cmd_reader.execute(cmd);
        }
        std::cout << ">> ";
    }
}