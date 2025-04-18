#pragma once

#include <iostream>

#include <latimer/lexical_analysis/token.hpp>

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg)
        : std::runtime_error(msg) {}
};

class LogicError : public std::runtime_error {
public:
    int line_;

    explicit LogicError(int line, const std::string& msg)
        : std::runtime_error(msg)
        , line_(line) {}
};

class TypeError : public std::runtime_error {
public:
    int line_;

    explicit TypeError(int line, const std::string& msg)
        : std::runtime_error(msg)
        , line_(line) {}
};

class RuntimeError : public std::runtime_error {
public:
    int line_;

    explicit RuntimeError(int line, const std::string& msg)
        : std::runtime_error(msg)
        , line_(line) {}
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

    void parseError(const Token& token, const std::string& msg) {
        if (token.type_ == TokenType::END_OF_FILE)
            report(token.line_, " at end of file", msg);
        else
            report(token.line_, " at '" + token.lexeme_ + "'", msg);
    }

    void parseError(int line, const std::string& msg) {
        report(line, "", msg);
    }

    void logicError(LogicError error) {
        std::cerr << "[line " << error.line_ << "] Logic Error: " << error.what() << std::endl;
        hadError_ = true;
    }

    void typeError(TypeError error) {
        std::cerr << "[line " << error.line_ << "] Type Error: " << error.what() << std::endl;
        hadError_ = true;
    }

    void runtimeError(RuntimeError error) {
        std::cerr << "[line " << error.line_ << "] Runtime Error: " << error.what() << std::endl;
        hadRuntimeError_ = true;
    }
};

}; // namespace Utils
