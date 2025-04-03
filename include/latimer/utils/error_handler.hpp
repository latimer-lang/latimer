#pragma once

#include <iostream>

#include <latimer/lexical_analysis/token.hpp>

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg)
        : std::runtime_error(msg) {}
};

class RuntimeError : public std::runtime_error {
public:
    Token token_;

    explicit RuntimeError(Token token, const std::string& msg)
        : std::runtime_error(msg)
        , token_(token) {}
};

class InternalCompilerError : public std::runtime_error {
public:

    explicit InternalCompilerError(const std::string& msg)
        : std::runtime_error(msg) {}
};

namespace Utils {

struct ErrorHandler {
public:
    bool hadError_;
    bool hadRuntimeError_;

    ErrorHandler()
        : hadError_(false)
        , hadRuntimeError_(false) {}

    void report(int line, const std::string& where, const std::string& msg) {
        std::cerr << "[line " << line << "] Error" << where << ": " + msg << std::endl;
        hadError_ = true;
    }

    void error(const Token& token, const std::string& msg) {
        if (token.type_ == TokenType::END_OF_FILE)
            report(token.line_, " at end of file", msg);
        else
            report(token.line_, " at '" + token.lexeme_ + "'", msg);
    }

    void error(int line, const std::string& msg) {
        report(line, "", msg);
    }

    void runtimeError(RuntimeError error) {
        std::cerr << "[line " << error.token_.line_ << "] Error: " << error.what() << std::endl;
        hadRuntimeError_ = true;
    }
};

}; // namespace Utils
