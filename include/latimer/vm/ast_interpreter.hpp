#pragma once

#include <variant>

#include <latimer/utils/error_handler.hpp>
#include <latimer/ast/ast.hpp>

class AstInterpreter : public AstVisitor {
public:
    explicit AstInterpreter(Utils::ErrorHandler& errorHandler);

    void interpret(AstExpr& expr);

private:
    using R = std::variant<std::monostate, bool, int32_t, float, std::string, char>;
    R result_;
    Utils::ErrorHandler& errorHandler_;

    R evaluate(AstExpr& expr);
    void visitGroupExpr(AstExprGroup& expr) override;
    void visitUnaryExpr(AstExprUnary& expr) override;
    void visitBinaryExpr(AstExprBinary& expr) override;
    void visitTernaryExpr(AstExprTernary& expr) override;
    void visitLiteralNullExpr(AstExprLiteralNull& expr) override;
    void visitLiteralBoolExpr(AstExprLiteralBool& expr) override;
    void visitLiteralIntExpr(AstExprLiteralInt& expr) override;
    void visitLiteralFloatExpr(AstExprLiteralFloat& expr) override;
    void visitLiteralStringExpr(AstExprLiteralString& expr) override;
    void visitLiteralCharExpr(AstExprLiteralChar& expr) override;
    inline std::string toString(R value);
};
