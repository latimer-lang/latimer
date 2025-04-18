#pragma once

#include <unordered_map>

#include <latimer/ast/ast.hpp>
#include <latimer/semantic_analysis/type.hpp>
#include <latimer/utils/error_handler.hpp>

class TypeEnvironment;
using TypeEnvironmentPtr = std::shared_ptr<TypeEnvironment>;

class TypeEnvironment {
public:
    std::unordered_map<std::string, TypePtr> assignedType_;
    std::unordered_map<std::string, TypePtr> declaredType_;
    TypeEnvironmentPtr enclosing_;

    explicit TypeEnvironment();
    explicit TypeEnvironment(TypeEnvironmentPtr enclosing);
    
    void declareAndAssign(const std::string& name, TypePtr type);
    void declare(const std::string& name, TypePtr type);
    TypePtr declaredType(const std::string& name);
    void assign(const std::string& name, TypePtr type);
    TypePtr assignedType(const std::string& name);
};

class TypeEnvironmentGuard {
public:
    TypeEnvironmentPtr& target_;
    TypeEnvironmentPtr previous_;

    TypeEnvironmentGuard(TypeEnvironmentPtr& env, TypeEnvironmentPtr newEnv)
        : target_(env)
        , previous_(env) {
        target_ = newEnv;
    }

    ~TypeEnvironmentGuard() {
        target_ = previous_;
    }
};

class Checker : public AstVisitor {
public:
    explicit Checker(Utils::ErrorHandler& errorHandler);

    void check(const std::vector<AstStatPtr>& statements);
    
private:
    TypePtr result_;
    Utils::ErrorHandler& errorHandler_;
    TypeEnvironmentPtr globals_;
    TypeEnvironmentPtr env_;
    int loopDepth_;
    TypePtr currFunctionRetTy_;

    void checkStat(AstStat& stat);
    TypePtr checkExpr(AstExpr& expr);
    TypePtr convertAstType(AstType& type);

    void visitPrimitiveType(AstTypePrimitive& type) override;
    void visitFunctionType(AstTypeFunction& type) override;

    void visitGroupExpr(AstExprGroup& expr) override;
    void visitUnaryExpr(AstExprUnary& expr) override;
    void visitBinaryExpr(AstExprBinary& expr) override;
    void visitTernaryExpr(AstExprTernary& expr) override;
    void visitLiteralNullExpr(AstExprLiteralNull& expr) override;
    void visitLiteralBoolExpr(AstExprLiteralBool& expr) override;
    void visitLiteralIntExpr(AstExprLiteralInt& expr) override;
    void visitLiteralDoubleExpr(AstExprLiteralDouble& expr) override;
    void visitLiteralStringExpr(AstExprLiteralString& expr) override;
    void visitLiteralCharExpr(AstExprLiteralChar& expr) override;
    void visitVariableExpr(AstExprVariable& expr) override;
    void visitAssignmentExpr(AstExprAssignment& expr) override;
    void visitCallExpr(AstExprCall& expr) override;

    void visitVarDeclStat(AstStatVarDecl& stat) override;
    void visitExpressionStat(AstStatExpression& stat) override;
    void visitIfElseStat(AstStatIfElse& stat) override;
    void visitWhileStat(AstStatWhile& stat) override;
    void visitForStat(AstStatFor& stat) override;
    void visitBreakStat(AstStatBreak& stat) override;
    void visitContinueStat(AstStatContinue& stat) override;
    void visitBlockStat(AstStatBlock& stat) override;
    void visitFuncDeclStat(AstStatFuncDecl& stat) override;
    void visitReturnStat(AstStatReturn& stat) override;
};
