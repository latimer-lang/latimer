#pragma once

#include <memory>
#include <string>
#include <vector>

#include <latimer/lexical_analysis/token.hpp>

class AstVisitor;

class AstNode {
    public:
    int line_;
    
    explicit AstNode(int line)
        : line_(line) {}
};

class AstExpr;
using AstExprPtr = std::unique_ptr<AstExpr>;

class AstExpr : public AstNode {
public:
    explicit AstExpr(int line)
        : AstNode(line) {}

    virtual ~AstExpr() = default;

    virtual void accept(AstVisitor& visitor) = 0;
};

class AstExprGroup : public AstExpr {
public:
    AstExprPtr expr_;

    explicit AstExprGroup(int line, AstExprPtr expr)
        : AstExpr(line)
        , expr_(std::move(expr)) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprUnary : public AstExpr {
public:
    Token op_;
    AstExprPtr right_;

    explicit AstExprUnary(int line, Token op, AstExprPtr right)
        : AstExpr(line)
        , op_(op)
        , right_(std::move(right)) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprBinary : public AstExpr {
public:
    AstExprPtr left_;
    Token op_;
    AstExprPtr right_;

    explicit AstExprBinary(int line, AstExprPtr left, Token op, AstExprPtr right)
        : AstExpr(line)
        , left_(std::move(left))
        , op_(op)
        , right_(std::move(right)) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprTernary : public AstExpr {
public:
    AstExprPtr condition_;
    AstExprPtr thenBranch_;
    AstExprPtr elseBranch_;

    explicit AstExprTernary(int line, AstExprPtr condition, AstExprPtr thenBranch,
                            AstExprPtr elseBranch)
        : AstExpr(line)
        , condition_(std::move(condition))
        , thenBranch_(std::move(thenBranch))
        , elseBranch_(std::move(elseBranch)) {}

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
    int32_t value_;

    explicit AstExprLiteralInt(int line, int32_t value)
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

class AstExprVariable : public AstExpr {
public:
    Token name_;

    explicit AstExprVariable(int line, Token name)
        : AstExpr(line)
        , name_(name) {}

    void accept(AstVisitor& visitor) override;
};

class AstExprAssignment : public AstExpr {
public:
    Token name_;
    AstExprPtr value_;

    explicit AstExprAssignment(int line, Token name, AstExprPtr value)
        : AstExpr(line)
        , name_(name)
        , value_(std::move(value)) {}

    void accept(AstVisitor& visitor) override;
};

class AstStat;
using AstStatPtr = std::unique_ptr<AstStat>;

class AstStat : public AstNode {
public:
    explicit AstStat(int line)
        : AstNode(line) {}

    virtual ~AstStat() = default;

    virtual void accept(AstVisitor& visitor) = 0;
};

class AstStatVarDecl : public AstStat {
public:
    Token type_;
    Token name_;
    AstExprPtr initializer_;

    explicit AstStatVarDecl(int line, Token type, Token name, AstExprPtr initializer)
        : AstStat(line)
        , type_(type)
        , name_(name)
        , initializer_(std::move(initializer)) {}

    void accept(AstVisitor& visitor) override;
};

class AstStatExpression : public AstStat {
public:
    AstExprPtr expr_;

    explicit AstStatExpression(int line, AstExprPtr expr)
        : AstStat(line)
        , expr_(std::move(expr)) {}
    
    void accept(AstVisitor& visitor) override;
};

class AstStatPrint : public AstStat {
public:
    AstExprPtr expr_;

    explicit AstStatPrint(int line, AstExprPtr expr)
        : AstStat(line)
        , expr_(std::move(expr)) {}

    void accept(AstVisitor& visitor) override;
};

class AstStatBlock : public AstStat {
public:
    std::vector<AstStatPtr> body_;

    explicit AstStatBlock(int line, std::vector<AstStatPtr> body)
        : AstStat(line)
        , body_(std::move(body)) {}
    
    void accept(AstVisitor& visitor) override;
};

class AstVisitor {
public:
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
    virtual void visitVariableExpr(AstExprVariable& expr) = 0;
    virtual void visitAssignmentExpr(AstExprAssignment& expr) = 0;

    virtual void visitVarDeclStat(AstStatVarDecl& stat) = 0;
    virtual void visitExpressionStat(AstStatExpression& stat) = 0;
    virtual void visitPrintStat(AstStatPrint& stat) = 0;
    virtual void visitBlockStat(AstStatBlock& stat) = 0;
};
