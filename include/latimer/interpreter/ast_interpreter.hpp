#pragma once

#include <exception>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <latimer/lexical_analysis/token.hpp>
#include <latimer/utils/error_handler.hpp>
#include <latimer/ast/ast.hpp>
#include <latimer/interpreter/value.hpp>

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment {
public:
    std::unordered_map<std::string, Runtime::Value> values_;
    std::unordered_set<std::string> declared_;
    EnvironmentPtr enclosing_;

    explicit Environment();
    explicit Environment(EnvironmentPtr enclosing);
    
    void declare(std::string name);
    bool isDeclared(std::string name);
    void define(std::string name, Runtime::Value value);
    void assign(Token name, Runtime::Value value);
    Runtime::Value get(Token name);
};

class EnvironmentGuard {
public:
    EnvironmentPtr& target_;
    EnvironmentPtr previous_;

    EnvironmentGuard(EnvironmentPtr& env, EnvironmentPtr newEnv)
        : target_(env)
        , previous_(env) {
            target_ = newEnv;
    }

    ~EnvironmentGuard() {
        target_ = previous_;
    }
};

class AstInterpreter : public AstVisitor {
public:
    explicit AstInterpreter(Utils::ErrorHandler& errorHandler);

    void interpret(const std::vector<AstStatPtr>& statements);

private:
    Runtime::Value result_;
    Utils::ErrorHandler& errorHandler_;
    EnvironmentPtr globals_;
    EnvironmentPtr env_;

    void execute(AstStat& stat);
    Runtime::Value evaluate(AstExpr& expr);

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

    struct BreakSignal : public std::exception {};
    struct ContinueSignal : public std::exception {};
    struct ReturnSignal : public std::exception {
        Runtime::Value value_;

        explicit ReturnSignal(Runtime::Value value)
            : std::exception()
            , value_(value) {}
    };
    bool requireBool(const Runtime::Value& value, int line, const std::string& errorMsg);
    void executeBlocK(const std::vector<AstStatPtr>& body, EnvironmentPtr localEnv);

    struct UserFunction : public Runtime::Callable {
        AstStatFuncDecl* decl_;
        EnvironmentPtr closure_;

        explicit UserFunction(AstStatFuncDecl* decl, EnvironmentPtr closer);

        size_t arity() const override;
        Runtime::Value call(int line, AstInterpreter& interpreter, const std::vector<Runtime::Value>& arguments) override;
        std::string toString() const override;
    };
};
