#include <iostream>
#include <cstring>
#include <fstream>
#include "TicketSystem.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    TicketSystem system;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        system.processCommand(line);
    }
    
    return 0;
}
