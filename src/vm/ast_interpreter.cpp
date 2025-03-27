#include <latimer/vm/ast_interpreter.hpp>

#include <iostream>
#include <stdexcept>
#include <variant>

#include "latimer/utils/error_handler.hpp"
#include <latimer/utils/macros.hpp>
#include "latimer/ast/ast.hpp"
#include <latimer/lexical_analysis/token.hpp>

AstInterpreter::AstInterpreter(Utils::ErrorHandler& errorHandler)
    : errorHandler_(errorHandler) {}

void AstInterpreter::interpret(AstExpr& expr) {
    try {
        R output = evaluate(expr);
        std::cout << toString(output) << std::endl;
    } catch (RuntimeError error) {
        errorHandler_.runtimeError(error);
    }
}

AstInterpreter::R AstInterpreter::evaluate(AstExpr& expr) {
    expr.accept(*this);
    return result_;
}

void AstInterpreter::visitGroupExpr(AstExprGroup& expr) {
    result_ = evaluate(*expr.expr_);
}

void AstInterpreter::visitUnaryExpr(AstExprUnary& expr) {
    R right = evaluate(*expr.right_);

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
            throw RuntimeError(expr.op_, "[Internal Compiler Error]: Unexpected Unary Operator: " + expr.op_.stringifyTokenType() + ".");
            break;
    }
}

void AstInterpreter::visitBinaryExpr(AstExprBinary& expr) {
    R left = evaluate(*expr.left_);
    R right = evaluate(*expr.right_);

    switch (expr.op_.type_) {
        case TokenType::SLASH: // TODO: Division by Zero error
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) / std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) / std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " / " + "'" + toString(right) + "'.");
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) * std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) * std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " * " + "'" + toString(right) + "'.");
            break;
        case TokenType::PERECENT:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) % std::get<int32_t>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " % " + "'" + toString(right) + "'.");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) - std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) - std::get<float>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " - " + "'" + toString(right) + "'.");
            break;
        case TokenType::PLUS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) + std::get<int32_t>(right);
            else if (std::holds_alternative<float>(left) && std::holds_alternative<float>(right))
                result_ = std::get<float>(left) + std::get<float>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                result_ = std::get<std::string>(left) + std::get<std::string>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " + " + "'" + toString(right) + "'.");
            break;
        case TokenType::GREATER_GREATER:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) >> std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) >> std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " >> " + "'" + toString(right) + "'.");
            break;
        case TokenType::LESS_LESS:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) << std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) << std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " << " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " > " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " >= " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " < " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " <= " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " == " + "'" + toString(right) + "'.");
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
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " != " + "'" + toString(right) + "'.");
            break;
        case TokenType::PIPE:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) | std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) | std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " | " + "'" + toString(right) + "'.");
            break;
        case TokenType::AMPERSAND:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) & std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) & std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " & " + "'" + toString(right) + "'.");
            break;
        case TokenType::CARET:
            if (std::holds_alternative<int32_t>(left) && std::holds_alternative<int32_t>(right))
                result_ = std::get<int32_t>(left) ^ std::get<int32_t>(right);
            else if (std::holds_alternative<char>(left) && std::holds_alternative<char>(right))
                result_ = std::get<char>(left) ^ std::get<char>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " ^ " + "'" + toString(right) + "'.");
            break;
        case TokenType::PIPE_PIPE: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) || std::get<bool>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " || " + "'" + toString(right) + "'.");
            break;
        case TokenType::AMPERSAND_AMPERSAND: // TODO: implement short circuiting
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                result_ = std::get<bool>(left) && std::get<bool>(right);
            else
                throw RuntimeError(expr.op_, "Unsupported operands for '" + toString(left) + "'" + " && " + "'" + toString(right) + "'.");
            break;
        default:
            throw RuntimeError(expr.op_, "[Internal Compiler Error]: Unexpected Binary Operator: " + expr.op_.stringifyTokenType() + ".");
    }
}

void AstInterpreter::visitTernaryExpr(AstExprTernary& expr) {
    R cond = evaluate(*expr.condition_);

    if (!std::holds_alternative<bool>(cond))
        throw RuntimeError({TokenType::QUESTION_MARK, "?", NULL, expr.line_}, "Ternary condition must be a boolean.");

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

inline std::string AstInterpreter::toString(R value) {
    if (std::holds_alternative<std::monostate>(value)) return "null";
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
    if (std::holds_alternative<int32_t>(value)) return std::to_string(std::get<int32_t>(value));
    if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    if (std::holds_alternative<char>(value)) return std::string(1, std::get<char>(value));
    return "<unknown>";
}