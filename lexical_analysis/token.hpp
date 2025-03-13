#pragma once

#include <string>
#include <variant>

enum TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One/two-character tokens
    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER,
    GREATER_EQUAL, LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END_OF_FILE,
};

struct Token {

    using Literal = std::variant<std::string, bool, int>;

    TokenType type_;
    std::string lexeme_;
    Literal literal_;
    int line_;

    Token(TokenType type, std::string lexeme, Literal literal, int line) : type_(type), lexeme_(lexeme), literal_(literal), line_(line) {}
};