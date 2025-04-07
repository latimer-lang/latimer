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

void Environment::declare(std::string name) {
    declared_.insert({name});
}

bool Environment::isDeclared(std::string name) {
    return declared_.find(name) != declared_.end();
}

void Environment::define(std::string name, Runtime::Value value) {
    values_.insert({name, value});
}

void Environment::assign(Token name, Runtime::Value value) {
    std::string lexeme = name.lexeme_;

    if (values_.find(lexeme) != values_.end()) {
        values_[lexeme] = value;
        return;
    }

    if (enclosing_ != nullptr) {
        enclosing_->assign(name, value);
        return;
    }

    if (isDeclared(lexeme)) {
        define(lexeme, value);
        return;
    }

    throw RuntimeError(name.line_, "Cannot assign value " + Runtime::toString(value) + " to undefined variable '" + name.lexeme_ + "'.");
}

Runtime::Value Environment::get(Token name) {
    if (values_.find(name.lexeme_) != values_.end())
        return values_.at(name.lexeme_);

    if (enclosing_ != nullptr)
        return enclosing_->get(name);

    throw RuntimeError(name.line_, "Variable '" + name.lexeme_ + "' has not been declared or initialized.");
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
                throw RuntimeError(expr.op_.line_, "Logical NOT expects boolean.");
            break;
        case TokenType::TILDE:
            if (std::holds_alternative<int32_t>(right))
                result_ = ~std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Bitwise NOT (~) expects integer.");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int32_t>(right))
                result_ = -std::get<int32_t>(right);
            else if (std::holds_alternative<float>(right))
                result_ = -std::get<float>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unary minus expects number.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " / " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) * std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) * std::get<float>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " * " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PERECENT:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) % std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " % " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) - std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) - std::get<float>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " - " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PLUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) + std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) + std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) + std::get<std::string>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " + " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::GREATER_GREATER:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) >> std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " >> " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::LESS_LESS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) << std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " << " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " > " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " >= " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " < " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " <= " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " == " + "'" + Runtime::toString(right) + "'.");
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
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " != " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PIPE:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) | std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " | " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::AMPERSAND:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) & std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " & " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::CARET:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) ^ std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " ^ " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::PIPE_PIPE: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) || std::get<bool>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " || " + "'" + Runtime::toString(right) + "'.");
            break;
        case TokenType::AMPERSAND_AMPERSAND: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) && std::get<bool>(right);
            else
                throw RuntimeError(expr.op_.line_, "Unsupported operands for '" + Runtime::toString(left) + "'" + " && " + "'" + Runtime::toString(right) + "'.");
            break;
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Binary Operator: " + expr.op_.stringifyTokenType() + ".");
    }
}

void AstInterpreter::visitTernaryExpr(AstExprTernary& expr) {
    Runtime::Value cond = evaluate(*expr.condition_);

    if (!std::holds_alternative<bool>(cond))
        throw RuntimeError(expr.line_, "Ternary condition must be a boolean.");

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
    std::string lexeme = stat.name_.lexeme_;
    env_->declare(lexeme);

    if (stat.initializer_ == nullptr)
        return;

    Runtime::Value value = evaluate(*stat.initializer_);
    env_->define(lexeme, value);
}

void AstInterpreter::visitExpressionStat(AstStatExpression& stat) {
    evaluate(*stat.expr_);
}

void AstInterpreter::visitIfElseStat(AstStatIfElse& stat) {
    if (requireBool(evaluate(*stat.condition_), stat.line_, "Condition of if statement must evaluate to a boolean value.")) {
        execute(*stat.thenBranch_);
        return;
    }

    if (stat.elseBranch_ != nullptr)
        execute(*stat.elseBranch_);
}

void AstInterpreter::visitWhileStat(AstStatWhile &stat) {
    while (requireBool(evaluate(*stat.condition_), stat.line_, "Condition of while loop must evaluate to a boolean value.")) {
        try {
            execute(*stat.body_);
        } catch (const ContinueSignal&) {
            // Skip to increment
        } catch (const BreakSignal&) {
            break;
        }
    }
}

void AstInterpreter::visitForStat(AstStatFor& stat) {
    if (stat.initializer_ != nullptr)
        execute(*stat.initializer_);

    while (true) {
        if (stat.condition_ != nullptr) {
            Runtime::Value conditionValue = evaluate(*stat.condition_);
            if (!requireBool(conditionValue, stat.line_, "For loop condition must evaluate to a boolean."))
                break;
        }

        try {
            execute(*stat.body_);
        } catch (const ContinueSignal&) {
            // Skip to increment
        } catch (const BreakSignal&) {
            break;
        }

        if (stat.increment_ != nullptr)
            evaluate(*stat.increment_);
    }
}

void AstInterpreter::visitBreakStat(AstStatBreak& stat) {
    throw BreakSignal();
}

void AstInterpreter::visitContinueStat(AstStatContinue& stat) {
    throw ContinueSignal();
}

void AstInterpreter::visitPrintStat(AstStatPrint& stat) {
    Runtime::Value output = evaluate(*stat.expr_);
    std::cout << Runtime::toString(output) << std::endl;
}

void AstInterpreter::visitBlockStat(AstStatBlock& stat) {
    executeBlocK(stat.body_, std::make_unique<Environment>(env_.get()));
}

bool AstInterpreter::requireBool(const Runtime::Value& value, int line, const std::string& errorMsg) {
    if (!std::holds_alternative<bool>(value))
        throw RuntimeError(line, errorMsg);
    return std::get<bool>(value);
}

void AstInterpreter::executeBlocK(const std::vector<AstStatPtr>& body, EnvironmentPtr localEnv) {
    // Very Important to have this guard bc it guarantees that our environment is properly restored when execute(...) throws an error
    // Consider this scenario in REPL:
    // > int a = 1;
    // > { int a = 2; some runtime error code }
    // > print a;
    // without the guard, prints 2 (which is incorrect); with guard, prints 1 (correct)
    EnvironmentGuard guard(env_, std::move(localEnv));

    for (const AstStatPtr& stat : body) {
        execute(*stat);
    }
}
