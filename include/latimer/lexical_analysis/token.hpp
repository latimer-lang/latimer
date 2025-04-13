#pragma once

#include <any>
#include <string>

#include <latimer/utils/macros.hpp>
#include <latimer/interpreter/value.hpp> // TODO: Kind of weird that value.hpp refers to interpreter. it may be better to create two versions of Value (one for static time and the other for runtime) and in the parser, we could translate

enum class TokenType : uint8_t {
    // Single-character tokens
    LEFT_PAREN = 1,
    RIGHT_PAREN = 2,
    LEFT_BRACE = 3,
    RIGHT_BRACE = 4,
    LEFT_BRACKET = 5,
    RIGHT_BRACKET = 6,
    COMMA = 7,
    DOT = 8,
    MINUS = 9,
    PLUS = 10,
    SEMICOLON = 11,
    COLON = 12,
    SLASH = 13,
    STAR = 14,
    PERECENT = 15,
    TILDE = 16,
    CARET = 17,
    QUESTION_MARK = 18,

    // One/two-character tokens
    BANG = 19,
    BANG_EQUAL = 20,
    EQUAL = 21,
    EQUAL_EQUAL = 22,
    GREATER = 23,
    GREATER_GREATER = 24,
    GREATER_EQUAL = 25,
    LESS = 26,
    LESS_LESS = 27,
    LESS_EQUAL = 28,
    AMPERSAND = 29,
    AMPERSAND_AMPERSAND = 30,
    PIPE = 31,
    PIPE_PIPE = 32,

    // Identifier
    IDENTIFIER = 33,

    // Literals
    CHARACTER_LIT = 34,
    STRING_LIT = 35,
    INTEGER_LIT = 36,
    DOUBLE_LIT = 37,
    TRUE_LIT = 38,
    FALSE_LIT = 39,

    // Keywords
    CLASS = 40,
    ELSE = 41,
    FOR = 42,
    IF = 43,
    NIL = 44, // "null" in Latimer, but NULL is reserved in C++
    RETURN = 45,
    SUPER = 46,
    THIS = 47,
    WHILE = 48,
    BREAK = 49,
    CONTINUE = 50,

    // Types
    BOOL_TY = 51,
    INT_TY = 52,
    DOUBLE_TY = 53,
    CHAR_TY = 54,
    STRING_TY = 55,
    VOID_TY = 56, // for function return types

    END_OF_FILE = 57,
};

struct Token {
    TokenType type_;
    std::string lexeme_;
    Runtime::Value literal_;
    int line_;

    Token(TokenType type, std::string lexeme, Runtime::Value literal, int line)
        : type_(type)
        , lexeme_(lexeme)
        , literal_(literal)
        , line_(line) {}

    std::string stringifyTokenType() {
        // clang-format off
        switch (type_) {
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case TokenType::LEFT_BRACE: return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
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
            case TokenType::DOUBLE_LIT: return "DOUBLE_LIT";
            case TokenType::TRUE_LIT: return "TRUE_LIT";
            case TokenType::FALSE_LIT: return "FALSE_LIT";
            case TokenType::CLASS: return "CLASS";
            case TokenType::ELSE: return "ELSE";
            case TokenType::FOR: return "FOR";
            case TokenType::IF: return "IF";
            case TokenType::NIL: return "NIL";
            case TokenType::RETURN: return "RETURN";
            case TokenType::SUPER: return "SUPER";
            case TokenType::THIS: return "THIS";
            case TokenType::WHILE: return "WHILE";
            case TokenType::BREAK: return "BREAK";
            case TokenType::CONTINUE: return "CONTINUE";
            case TokenType::BOOL_TY: return "BOOL_TY";
            case TokenType::INT_TY: return "INT_TY";
            case TokenType::DOUBLE_TY: return "DOUBLE_TY";
            case TokenType::CHAR_TY: return "CHAR_TY";
            case TokenType::STRING_TY: return "STRING_TY";
            case TokenType::VOID_TY: return "VOID_TY";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
}
        // clang-format on

        UNREACHABLE_CODE
        return "";
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
