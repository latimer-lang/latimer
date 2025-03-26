#pragma once

#include <iostream>

#include <latimer/lexical_analysis/token.hpp>

namespace Utils {

struct ErrorHandler {
public:
    bool hadError_;

    ErrorHandler()
        : hadError_(false) {}

    void report(int line, const std::string& where, const std::string& msg) {
        std::cerr << "[line " << line << "] Error" << where << ": " + msg << std::endl;
        hadError_ = true;
    }

    void error(const Token& token, const std::string& msg) {
        if (token.type_ == TokenType::END_OF_FILE)
            report(token.line_, " at end of file", msg);
        else
            report(token.line_, " at '" + token.lexeme_ + "'" , msg);
    }

    void error(int line, const std::string& msg) {
        report(line, "", msg);
    }
};

}; // namespace Utils
