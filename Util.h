#pragma once
// ---------------------------------------------------------------------------
// Util.h - small console input helpers used across all modules.
// These are plain I/O helpers (not data structures), so they are allowed.
// ---------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstdlib>

inline int readInt(const std::string& prompt) {
    std::cout << prompt;
    long long x;
    while (!(std::cin >> x)) {
        if (std::cin.eof()) {        // input closed (e.g. piped) - quit cleanly
            std::cout << "\n(end of input)\n";
            std::exit(0);
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "  Invalid number, try again: ";
    }
    std::cin.ignore(10000, '\n'); // drop rest of the line so getline works next
    return (int)x;
}

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

inline void pause() {
    std::cout << "\n(Press Enter to continue...)";
    std::string s;
    std::getline(std::cin, s);
}

inline void line(char c = '-', int n = 60) {
    for (int i = 0; i < n; i++) std::cout << c;
    std::cout << "\n";
}
