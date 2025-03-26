#pragma once

#include <any>
#include <string>
#include <typeinfo>

#include "../utils/macros.hpp"

enum class TokenType : uint8_t {
    // Single-character tokens
    LEFT_PAREN = 1,
    RIGHT_PAREN = 2,
    LEFT_BRACE = 3,
    RIGHT_BRACE = 4,
    COMMA = 5,
    DOT = 6,
    MINUS = 7,
    PLUS = 8,
    SEMICOLON = 9,
    SLASH = 10,
    STAR = 11,

    // One/two-character tokens
    BANG = 12,
    BANG_EQUAL = 13,
    EQUAL = 14,
    EQUAL_EQUAL = 15,
    GREATER = 16,
    GREATER_EQUAL = 17,
    LESS = 18,
    LESS_EQUAL = 19,

    // Literals
    IDENTIFIER = 20,
    CHARACTER = 21,
    STRING = 22,
    NUMBER = 23,

    // Keywords
    AND = 24,
    CLASS = 25,
    ELSE = 26,
    FALSE_ = 27,
    FUN = 28,
    FOR = 29,
    IF = 30,
    NIL = 31,
    OR = 32,
    PRINT = 33,
    RETURN = 34,
    SUPER = 35,
    THIS = 36,
    TRUE_ = 37,
    WHILE = 38,

    // Types
    BOOL_TY = 39,
    INT_TY = 40,
    FLOAT_TY = 41,
    CHAR_TY = 42,
    STRING_TY = 43,

    END_OF_FILE,
};

struct Token {
    TokenType type_;
    std::string lexeme_;
    std::any literal_;
    int line_;

    Token(TokenType type, std::string lexeme, std::any literal, int line)
        : type_(type)
        , lexeme_(lexeme)
        , literal_(literal)
        , line_(line) {}

    std::string stringifyType() {
        // clang-format off
        switch (type_) {
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case TokenType::LEFT_BRACE: return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case TokenType::COMMA: return "COMMA";
            case TokenType::DOT: return "DOT";
            case TokenType::MINUS: return "MINUS";
            case TokenType::PLUS: return "PLUS";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::SLASH: return "SLASH";
            case TokenType::STAR: return "STAR";
            case TokenType::BANG: return "BANG";
            case TokenType::BANG_EQUAL: return "BANG_EQUAL";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
            case TokenType::GREATER: return "GREATER";
            case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
            case TokenType::LESS: return "LESS";
            case TokenType::LESS_EQUAL: return "LESS_EQUAL";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::CHARACTER: return "CHARACTER";
            case TokenType::STRING: return "STRING";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::AND: return "AND";
            case TokenType::CLASS: return "CLASS";
            case TokenType::ELSE: return "ELSE";
            case TokenType::FALSE_: return "FALSE";
            case TokenType::FUN: return "FUN";
            case TokenType::FOR: return "FOR";
            case TokenType::IF: return "IF";
            case TokenType::NIL: return "NIL";
            case TokenType::OR: return "OR";
            case TokenType::PRINT: return "PRINT";
            case TokenType::RETURN: return "RETURN";
            case TokenType::SUPER: return "SUPER";
            case TokenType::THIS: return "THIS";
            case TokenType::TRUE_: return "TRUE";
            case TokenType::WHILE: return "WHILE";
            case TokenType::BOOL_TY: return "BOOL_TY";
            case TokenType::INT_TY: return "INT_TY";
            case TokenType::FLOAT_TY: return "FLOAT_TY";
            case TokenType::CHAR_TY: return "CHAR_TY";
            case TokenType::STRING_TY: return "STRING_TY";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
        }
        // clang-format on

        UNREACHABLE_CODE
        return "";
    }

    std::string stringifyLiteral() {
        if (literal_.type() == typeid(int))
            return std::to_string(std::any_cast<int>(literal_));
        else if (literal_.type() == typeid(double))
            return std::to_string(std::any_cast<double>(literal_));
        else if (literal_.type() == typeid(std::string))
            return std::any_cast<std::string>(literal_);
        else if (literal_.type() == typeid(bool))
            return std::to_string(std::any_cast<bool>(literal_));
        else if (literal_.type() == typeid(char))
            return escapeChar(std::any_cast<char>(literal_));
        else
            return "NULL";
    }

    std::string escapeChar(char c) {
        switch (c) {
            case '\n': return "\\n";
            case '\t': return "\\t";
            case '\r': return "\\r";
            case '\b': return "\\b";
            case '\f': return "\\f";
            case '\v': return "\\v";
            case '\\': return "\\\\";
            case '\'': return "\\\'";
            case '\"': return "\\\"";
            case '\0': return "\\0";
            default:
                if (std::isprint(static_cast<unsigned char>(c)))
                    return std::string(1, c);
                else {
                    char buf[5];
                    std::snprintf(buf, sizeof(buf), "\\x%02x", static_cast<unsigned char>(c));
                    return buf;
                }
        }
    }
};
