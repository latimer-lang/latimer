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

    AstTypePtr type();
    AstTypePtr funcTypeTail(AstTypePtr returnType);

    AstExprPtr expression();
    AstExprPtr assignment();
    AstExprPtr ternary();
    AstExprPtr logical();
    AstExprPtr bitwise();
    AstExprPtr equality();
    AstExprPtr comparison();
    AstExprPtr bitshift();
    AstExprPtr term();
    AstExprPtr factor();
    AstExprPtr unary();
    AstExprPtr call();
    AstExprPtr primary();

    AstStatPtr declaration();
    AstStatPtr statement();
    AstStatPtr varDeclStat();
    AstStatPtr varDeclStat(AstTypePtr declType, Token name);
    AstStatPtr funcDeclStat(AstTypePtr declType, Token name);
    AstStatPtr exprStat();
    AstStatPtr ifElseStat();
    AstStatPtr whileStat();
    AstStatPtr forStat();
    AstStatPtr breakStat();
    AstStatPtr continueStat();
    AstStatPtr returnStat();
    AstStatPtr blockStat();

    bool match(std::initializer_list<TokenType> types);
    bool check(std::initializer_list<TokenType> types);
    Token advance();
    bool isAtFront();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string msg);
    Token consume(std::initializer_list<TokenType> types, std::string msg);
    ParseError error(const Token& token, const std::string& msg);
    void synchronize();
};
