#pragma once

#include <vector>
#include <unordered_map>

#include <latimer/lexical_analysis/token.hpp>
#include <latimer/utils/error_handler.hpp>
#include <latimer/ast/ast.hpp>
#include <latimer/vm/value.hpp>

class Environment {
public:
    std::unordered_map<std::string, Runtime::Value> values;

    explicit Environment();
    
    void define(std::string, Runtime::Value value);
    Runtime::Value get(Token name);
};

class AstInterpreter : public AstVisitor {
public:
    explicit AstInterpreter(Utils::ErrorHandler& errorHandler, Environment& env);

    void interpret(const std::vector<AstStatPtr>& statements);

private:
    Runtime::Value result_;
    Utils::ErrorHandler& errorHandler_;
    Environment env_;

    void execute(AstStat& stat);
    Runtime::Value evaluate(AstExpr& expr);
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
    void visitVariableExpr(AstExprVariable& expr) override;

    void visitVarDeclStat(AstStatVarDecl& stat) override;
    void visitExpressionStat(AstStatExpression& stat) override;
    void visitPrintStat(AstStatPrint& stat) override;

    inline std::string toString(Runtime::Value value);
};
