#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "latimer/utils/error_handler.hpp"
#include "token.hpp"

struct Lexer {
public:
    Lexer(std::string src, Utils::ErrorHandler& errorHandler);

    std::vector<Token> scanTokens();

private:
    std::string src_;
    std::vector<Token> tokens_;
    int start_;
    int current_;
    int line_;
    std::unordered_map<std::string, TokenType> keywords_;
    Utils::ErrorHandler errorHandler_;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::any literal);
    bool match(char expected);
    char peek();
    void character();
    void string();
    bool isDigit(char c);
    char peekNext();
    void number();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    void identifier();
    void scanToken();
};