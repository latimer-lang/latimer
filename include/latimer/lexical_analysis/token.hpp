#pragma once

#include <any>
#include <string>
#include <typeinfo>

#include "latimer/utils/macros.hpp"

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
    COLON = 10,
    SLASH = 11,
    STAR = 12,
    PERECENT = 13,
    TILDE = 14,
    CARET = 15,
    QUESTION_MARK = 16,

    // One/two-character tokens
    BANG = 17,
    BANG_EQUAL = 18,
    EQUAL = 19,
    EQUAL_EQUAL = 20,
    GREATER = 21,
    GREATER_GREATER = 22,
    GREATER_EQUAL = 23,
    LESS = 24,
    LESS_LESS = 25,
    LESS_EQUAL = 26,
    AMPERSAND = 27,
    AMPERSAND_AMPERSAND = 28,
    PIPE = 29,
    PIPE_PIPE = 30,

    // Identifier
    IDENTIFIER = 31,

    // Literals
    CHARACTER_LIT = 32,
    STRING_LIT = 33,
    INTEGER_LIT = 34,
    FLOAT_LIT = 35,
    TRUE_LIT = 36,
    FALSE_LIT = 37,

    // Keywords
    CLASS = 38,
    ELSE = 39,
    FOR = 40,
    IF = 41,
    NIL = 42,
    PRINT = 43,
    RETURN = 44,
    SUPER = 45,
    THIS = 46,
    WHILE = 47,

    // Types
    BOOL_TY = 48,
    INT_TY = 49,
    FLOAT_TY = 50,
    CHAR_TY = 51,
    STRING_TY = 52,
    VOID_TY = 53, // for function return types

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
            case TokenType::COLON: return "COLON";
            case TokenType::SLASH: return "SLASH";
            case TokenType::STAR: return "STAR";
            case TokenType::PERECENT: return "PERCENT";
            case TokenType::TILDE: return "TILDE";
            case TokenType::CARET: return "CARET";
            case TokenType::QUESTION_MARK: return "QUESTION_MARK";
            case TokenType::BANG: return "BANG";
            case TokenType::BANG_EQUAL: return "BANG_EQUAL";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
            case TokenType::GREATER: return "GREATER";
            case TokenType::GREATER_GREATER: return "GREATER_GREATER";
            case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
            case TokenType::LESS: return "LESS";
            case TokenType::LESS_LESS: return "LESS_LESS";
            case TokenType::LESS_EQUAL: return "LESS_EQUAL";
            case TokenType::AMPERSAND: return "AMPERSAND";
            case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
            case TokenType::PIPE: return "PIPE";
            case TokenType::PIPE_PIPE: return "PIPE_PIPE";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::CHARACTER_LIT: return "CHARACTER_LIT";
            case TokenType::STRING_LIT: return "STRING_LIT";
            case TokenType::INTEGER_LIT: return "INTEGER_LIT";
            case TokenType::FLOAT_LIT: return "FLOAT_LIT";
            case TokenType::TRUE_LIT: return "TRUE_LIT";
            case TokenType::FALSE_LIT: return "FALSE_LIT";
            case TokenType::CLASS: return "CLASS";
            case TokenType::ELSE: return "ELSE";
            case TokenType::FOR: return "FOR";
            case TokenType::IF: return "IF";
            case TokenType::NIL: return "NIL";
            case TokenType::PRINT: return "PRINT";
            case TokenType::RETURN: return "RETURN";
            case TokenType::SUPER: return "SUPER";
            case TokenType::THIS: return "THIS";
            case TokenType::WHILE: return "WHILE";
            case TokenType::BOOL_TY: return "BOOL_TY";
            case TokenType::INT_TY: return "INT_TY";
            case TokenType::FLOAT_TY: return "FLOAT_TY";
            case TokenType::CHAR_TY: return "CHAR_TY";
            case TokenType::STRING_TY: return "STRING_TY";
            case TokenType::VOID_TY: return "VOID_TY";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
}
        // clang-format on

        UNREACHABLE_CODE
        return "";
    }

    std::string stringifyLiteral() {
        if (literal_.type() == typeid(int))
            return std::to_string(std::any_cast<int>(literal_));
        else if (literal_.type() == typeid(float))
            return std::to_string(std::any_cast<float>(literal_));
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
            case '\n':
                return "\\n";
            case '\t':
                return "\\t";
            case '\r':
                return "\\r";
            case '\b':
                return "\\b";
            case '\f':
                return "\\f";
            case '\v':
                return "\\v";
            case '\\':
                return "\\\\";
            case '\'':
                return "\\\'";
            case '\"':
                return "\\\"";
            case '\0':
                return "\\0";
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
