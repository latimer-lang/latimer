#pragma once

#include <sstream>

#include "latimer/ast/ast.hpp"

class AstPrinter : public AstVisitor {
public:
    std::string print(AstExpr& expr) {
        expr.accept(*this);
        return result_;
    }

    void visitGroupExpr(AstExprGroup& expr) override {
        result_ = "(group " + print(*expr.expr_) + ")";
    }

    void visitUnaryExpr(AstExprUnary& expr) override {
        result_ = "(" + expr.op_.lexeme_ + " " + print(*expr.right_) + ")";
    }

    void visitBinaryExpr(AstExprBinary& expr) override {
        result_ = "(" + expr.op_.lexeme_ + " " + print(*expr.left_) + " " + print(*expr.right_) + ")";
    }

    void visitTernaryExpr(AstExprTernary& expr) override {
        result_ = "(?: " + print(*expr.condition_) + " " + print(*expr.thenBranch_) + " " +
                  print(*expr.elseBranch_) + ")";
    }

    void visitLiteralNullExpr(AstExprLiteralNull& expr) override {
        result_ = "null";
    }

    void visitLiteralBoolExpr(AstExprLiteralBool& expr) override {
        result_ = expr.value_ ? "true" : "false";
    }

    void visitLiteralIntExpr(AstExprLiteralInt& expr) override {
        result_ = std::to_string(expr.value_);
    }

    void visitLiteralFloatExpr(AstExprLiteralFloat& expr) override {
        std::ostringstream ss;
        ss << expr.value_;
        result_ = ss.str();
    }

    void visitLiteralStringExpr(AstExprLiteralString& expr) override {
        result_ = "\"" + expr.value_ + "\"";
    }

    void visitLiteralCharExpr(AstExprLiteralChar& expr) override {
        result_ = "'" + std::string(1, expr.value_) + "'";
    }
};
