#pragma once

#include "../lexical_analysis/token.hpp"

class AstNode {
public:
    int line_;

    explicit AstNode(int line) : line_(line) {}
};

class AstExpr : public AstNode {
public:
    explicit AstExpr(int line) : AstNode(line) {}
};

class AstExprBinary : public AstExpr {
public:
    AstExpr& left_;
    Token& op_;
    AstExpr& right_;

    explicit AstExprBinary(int line, AstExpr& left, Token& op, AstExpr& right) : AstExpr(line), left_(left), op_(op), right_(right) { }
};
