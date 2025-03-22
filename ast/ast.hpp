#pragma once

#include "../lexical_analysis/token.hpp"

class AstNode {
public:
    int line_;

    explicit AstNode(int line)
        : line_(line) {}
};

class AstExpr : public AstNode {
public:
    explicit AstExpr(int line)
        : AstNode(line) {}
};

class AstExprGroup : public AstExpr {
public:
    AstExpr& expr_;

    explicit AstExprGroup(int line, AstExpr& expr)
        : AstExpr(line)
        , expr_(expr) {}
};

class AstExprUnary : public AstExpr {
public:
    Token& op_;
    AstExpr& right_;

    explicit AstExprUnary(int line, Token& op, AstExpr& right)
        : AstExpr(line)
        , op_(op)
        , right_(right) {}
};

class AstExprBinary : public AstExpr {
public:
    AstExpr& left_;
    Token& op_;
    AstExpr& right_;

    explicit AstExprBinary(int line, AstExpr& left, Token& op, AstExpr& right)
        : AstExpr(line)
        , left_(left)
        , op_(op)
        , right_(right) {}
};

class AstExprTernary : public AstExpr {
public:
    AstExpr& condition_;
    AstExpr& thenBranch_;
    AstExpr& elseBranch_;

    explicit AstExprTernary(int line, AstExpr& condition, AstExpr& thenBranch, AstExpr& elseBranch)
        : AstExpr(line)
        , condition_(condition)
        , thenBranch_(thenBranch)
        , elseBranch_(elseBranch) {}
};

class AstExprLiteralNull : public AstExpr {
public:
    explicit AstExprLiteralNull(int line)
        : AstExpr(line) {}
};

class AstExprLiteralBool : public AstExpr {
public:
    bool value_;

    explicit AstExprLiteralBool(int line, bool value)
        : AstExpr(line)
        , value_(value) {}
};

class AstExprLiteralInt : public AstExpr {
public:
    int value_;

    explicit AstExprLiteralInt(int line, int value)
        : AstExpr(line)
        , value_(value) {}
};

class AstExprLiteralFloat : public AstExpr {
public:
    float value_;

    explicit AstExprLiteralFloat(int line, float value)
        : AstExpr(line)
        , value_(value) {}
};

class AstExprLiteralString : public AstExpr {
public:
    std::string value_;

    explicit AstExprLiteralString(int line, std::string value)
        : AstExpr(line)
        , value_(value) {}
};

class AstExprLiteralChar : public AstExpr {
public:
    char value_;

    explicit AstExprLiteralChar(int line, char value)
        : AstExpr(line)
        , value_(value) {}
};
