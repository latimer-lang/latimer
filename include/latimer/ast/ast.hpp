#pragma once

#include <string>

#include "latimer/lexical_analysis/token.hpp"

class AstVisitor;

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

    virtual ~AstExpr() = default;

    virtual void accept(AstVisitor& visitor) = 0;
};

class AstExprGroup : public AstExpr {
public:
    AstExpr& expr_;

    explicit AstExprGroup(int line, AstExpr& expr)
        : AstExpr(line)
        , expr_(expr) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprUnary : public AstExpr {
public:
    Token& op_;
    AstExpr& right_;

    explicit AstExprUnary(int line, Token& op, AstExpr& right)
        : AstExpr(line)
        , op_(op)
        , right_(right) {}

    void accept(AstVisitor& visitor) override;
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

    void accept(AstVisitor& visitor) override;
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

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralNull : public AstExpr {
public:
    explicit AstExprLiteralNull(int line)
        : AstExpr(line) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralBool : public AstExpr {
public:
    bool value_;

    explicit AstExprLiteralBool(int line, bool value)
        : AstExpr(line)
        , value_(value) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralInt : public AstExpr {
public:
    int value_;

    explicit AstExprLiteralInt(int line, int value)
        : AstExpr(line)
        , value_(value) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralFloat : public AstExpr {
public:
    float value_;

    explicit AstExprLiteralFloat(int line, float value)
        : AstExpr(line)
        , value_(value) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralString : public AstExpr {
public:
    std::string value_;

    explicit AstExprLiteralString(int line, std::string value)
        : AstExpr(line)
        , value_(value) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprLiteralChar : public AstExpr {
public:
    char value_;

    explicit AstExprLiteralChar(int line, char value)
        : AstExpr(line)
        , value_(value) {}

    void accept(AstVisitor& visitor) override;
};

class AstVisitor {
public:
    std::string result_;

    ~AstVisitor() = default;

    virtual void visitGroupExpr(AstExprGroup& expr) = 0;
    virtual void visitUnaryExpr(AstExprUnary& expr) = 0;
    virtual void visitBinaryExpr(AstExprBinary& expr) = 0;
    virtual void visitTernaryExpr(AstExprTernary& expr) = 0;
    virtual void visitLiteralNullExpr(AstExprLiteralNull& expr) = 0;
    virtual void visitLiteralBoolExpr(AstExprLiteralBool& expr) = 0;
    virtual void visitLiteralIntExpr(AstExprLiteralInt& expr) = 0;
    virtual void visitLiteralFloatExpr(AstExprLiteralFloat& expr) = 0;
    virtual void visitLiteralStringExpr(AstExprLiteralString& expr) = 0;
    virtual void visitLiteralCharExpr(AstExprLiteralChar& expr) = 0;
};
