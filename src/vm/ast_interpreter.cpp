#include "latimer/utils/error_handler.hpp"
#include <latimer/vm/ast_interpreter.hpp>

#include <iostream>
#include <stdexcept>

#include <latimer/utils/macros.hpp>

Environment::Environment()
    : values_()
    , enclosing_() {}

Environment::Environment(Environment* enclosing)
    : values_()
    , enclosing_(enclosing) {}

void Environment::define(std::string name, Runtime::Value value) {
    values_.insert({name, value});
}

void Environment::assign(Token name, Runtime::Value value) {
    if (values_.find(name.lexeme_) != values_.end()) {
        values_[name.lexeme_] = value;
        return;
    }

    if (enclosing_ != NULL) {
        enclosing_->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Cannot assign value " + Runtime::toString(value) + " to undefined variable '" + name.lexeme_ + "'.");
}

Runtime::Value Environment::get(Token name) {
    if (values_.find(name.lexeme_) != values_.end())
        return values_.at(name.lexeme_);

    if (enclosing_ != NULL)
        return enclosing_->get(name);

    throw RuntimeError(name, "Variable '" + name.lexeme_ + "' has not been declared.");
}

AstInterpreter::AstInterpreter(Utils::ErrorHandler& errorHandler, EnvironmentPtr env)
    : errorHandler_(errorHandler)
    , env_(std::move(env)) {}

void AstInterpreter::interpret(const std::vector<AstStatPtr>& statements) {
    try {
        for (const AstStatPtr& stat : statements) {
            if (!stat) {
                throw InternalCompilerError("[Internal Compiler Error]: nullptr statement in AST list.");
            }
            execute(*stat);
        }
    } catch (RuntimeError error) {
        errorHandler_.runtimeError(error);
    } catch (InternalCompilerError error) {
        std::cerr << error.what() << std::endl;
    }
}

void AstInterpreter::execute(AstStat& stat) {
    stat.accept(*this);
}

Runtime::Value AstInterpreter::evaluate(AstExpr& expr) {
    expr.accept(*this);
    return result_;
}

void AstInterpreter::visitGroupExpr(AstExprGroup& expr) {
    result_ = evaluate(*expr.expr_);
}

void AstInterpreter::visitUnaryExpr(AstExprUnary& expr) {
    Runtime::Value right = evaluate(*expr.right_);

    switch (expr.op_.type_) {
        case TokenType::BANG:
            if (std::holds_alternative<bool>(right))
                result_ = !std::get<bool>(right);
            else if (std::holds_alternative<std::monostate>(right))
                result_ = true;
            else
                throw RuntimeError(expr.op_, "Logical NOT expects boolean.");
            break;
        case TokenType::TILDE:
            if (std::holds_alternative<int32_t>(right))
                result_ = ~std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_, "Bitwise NOT (~) expects integer.");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int32_t>(right))
                result_ = -std::get<int32_t>(right);
            else if (std::holds_alternative<float>(right))
                result_ = -std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unary minus expects number.");
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Unary Operator: " + expr.op_.stringifyTokenType() + ".");
            break;
    }
}

void AstInterpreter::visitBinaryExpr(AstExprBinary& expr) {
    Runtime::Value left = evaluate(*expr.left_);
    Runtime::Value right = evaluate(*expr.right_);

    switch (expr.op_.type_) {
        case TokenType::SLASH: // TODO: Division by Zero error
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) / std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) / std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " / " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) * std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) * std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " * " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PERECENT:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) % std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " % " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) - std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) - std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " - " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PLUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) + std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) + std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) + std::get<std::string>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " + " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::GREATER_GREATER:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) >> std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) >> std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " >> " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::LESS_LESS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) << std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) << std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " << " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::GREATER: // TODO: Allow comparison between int and float?
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) > std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) > std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) > std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) > std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " > " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::GREATER_EQUAL: // TODO: Allow comparison between int and float?
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) >= std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) >= std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) >= std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) >= std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " >= " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::LESS: // TODO: Allow comparison between int and float?
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) < std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) < std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) < std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) < std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " < " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::LESS_EQUAL: // TODO: Allow comparison between int and float?
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) <= std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) <= std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) <= std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) <= std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " <= " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::EQUAL_EQUAL:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) == std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) == std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) == std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) == std::get<char>(right);
            else if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) == std::get<bool>(right);
            else if (std::holds_alternative<std::monostate>(left) && std::holds_alternative<std::monostate>(right))
                result_ = std::get<std::monostate>(left) == std::get<std::monostate>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " == " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::BANG_EQUAL:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) != std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) != std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) != std::get<std::string>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) != std::get<char>(right);
            else if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) != std::get<bool>(right);
            else if (std::holds_alternative<std::monostate>(left) && std::holds_alternative<std::monostate>(right))
                result_ = std::get<std::monostate>(left) != std::get<std::monostate>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " != " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PIPE:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) | std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) | std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " | " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::AMPERSAND:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) & std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) & std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " & " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::CARET:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) ^ std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) ^ std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " ^ " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PIPE_PIPE: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) || std::get<bool>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " || " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::AMPERSAND_AMPERSAND: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) && std::get<bool>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " && " + "'" + Runtime::toString(right) + "'.");
            break;
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Binary Operator: " + expr.op_.stringifyTokenType() + ".");
    }
}

void AstInterpreter::visitTernaryExpr(AstExprTernary& expr) {
    Runtime::Value cond = evaluate(*expr.condition_);

    if (!std::holds_alternative<bool>(cond))
        throw RuntimeError({TokenType::QUESTION_MARK, "?", std::monostate{}, expr.line_}, "Ternary condition must be a boolean.");

    result_ = std::get<bool>(cond) ? evaluate(*expr.thenBranch_) : evaluate(*expr.elseBranch_);
}

void AstInterpreter::visitLiteralNullExpr(AstExprLiteralNull& expr) {
    result_ = std::monostate{};
}

void AstInterpreter::visitLiteralBoolExpr(AstExprLiteralBool& expr) {
    result_ = expr.value_;
}

void AstInterpreter::visitLiteralIntExpr(AstExprLiteralInt& expr) {
    result_ = expr.value_;
}

void AstInterpreter::visitLiteralFloatExpr(AstExprLiteralFloat& expr) {
    result_ = expr.value_;
}

void AstInterpreter::visitLiteralStringExpr(AstExprLiteralString& expr) {
    result_ = expr.value_;
}

void AstInterpreter::visitLiteralCharExpr(AstExprLiteralChar& expr) {
    result_ = expr.value_;
}

void AstInterpreter::visitVariableExpr(AstExprVariable& expr) {
    result_ = env_->get(expr.name_);
}

void AstInterpreter::visitAssignmentExpr(AstExprAssignment& expr) {
    Runtime::Value value = evaluate(*expr.value_);
    env_->assign(expr.name_, value);
    result_ = value;
}

void AstInterpreter::visitVarDeclStat(AstStatVarDecl& stat) {
    Runtime::Value value = std::monostate{};
    if (stat.initializer_ != NULL)
        value = evaluate(*stat.initializer_);

    env_->define(stat.name_.lexeme_, value);
}

void AstInterpreter::visitExpressionStat(AstStatExpression& stat) {
    evaluate(*stat.expr_);
}

void AstInterpreter::visitPrintStat(AstStatPrint& stat) {
    Runtime::Value output = evaluate(*stat.expr_);
    std::cout << Runtime::toString(output) << std::endl;
}

void AstInterpreter::visitBlockStat(AstStatBlock& stat) {
    executeBlocK(stat.body_, std::make_unique<Environment>(env_.get()));
}

void AstInterpreter::executeBlocK(const std::vector<AstStatPtr>& body, EnvironmentPtr localEnv) {
    EnvironmentPtr previous = std::move(env_); // TODO: add environment guard. If execute(...) throws an error we need to make sure to restore environment properly
    env_ = std::move(localEnv);

    for (const AstStatPtr& stat : body)
        execute(*stat);

    env_ = std::move(previous);
}
