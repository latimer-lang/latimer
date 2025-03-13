#pragma once

#include <iostream>

namespace Utils {

struct ErrorHandler {
public:
    bool hadError_;

    ErrorHandler() : hadError_(false) {}

    void report(int line, std::string where, std::string msg) {
        std::cerr << "[line " << line << "] Error " << where << ": " + msg << std::endl;
        hadError_ = true;
    }

    void error(int line, std::string msg) {
        report(line, "", msg);
    }
};

}; // namespace Utils
