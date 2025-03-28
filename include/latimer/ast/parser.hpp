#pragma once

#include <initializer_list>
#include <stdexcept>
#include <vector>

#include <latimer/ast/ast.hpp>
#include <latimer/lexical_analysis/token.hpp>
#include <latimer/utils/error_handler.hpp>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens, Utils::ErrorHandler& errorHandler);

    std::vector<AstStatPtr> parse();

private:
    std::vector<Token> tokens_;
    int current_;
    Utils::ErrorHandler& errorHandler_;

    AstExprPtr expression();
    AstExprPtr ternary();
    AstExprPtr logical();
    AstExprPtr bitwise();
    AstExprPtr equality();
    AstExprPtr comparison();
    AstExprPtr bitshift();
    AstExprPtr term();
    AstExprPtr factor();
    AstExprPtr unary();
    AstExprPtr primary();

    AstStatPtr declaration();
    AstStatPtr statement();
    AstStatPtr varDeclStat();
    AstStatPtr exprStat();
    AstStatPtr printStat();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string msg);
    Token consume(std::initializer_list<TokenType> types, std::string msg);
    ParseError error(const Token& token, const std::string& msg);
    void synchronize();
};
