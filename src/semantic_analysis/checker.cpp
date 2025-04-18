#include <latimer/semantic_analysis/checker.hpp>

#include <latimer/semantic_analysis/type.hpp>
#include <variant>

TypeEnvironment::TypeEnvironment()
    : assignedType_()
    , declaredType_()
    , enclosing_() {
    
    // Native functions
    declareAndAssign("print", std::make_shared<Type>(
        FunctionType(
            std::make_shared<Type>(PrimitiveType(PrimitiveType::Void)),
            std::vector<TypePtr>{
                std::make_shared<Type>(UnionType({
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::Integer)),
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::Double)),
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::String)),
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::Boolean)),
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::Character)),
                    std::make_shared<Type>(PrimitiveType(PrimitiveType::NilType))
                }))
            }
        )
    ));
    declareAndAssign("clock", std::make_shared<Type>(
        FunctionType(
            std::make_shared<Type>(PrimitiveType(PrimitiveType::Double)),
            std::vector<TypePtr>{}
        )
    ));
    declareAndAssign("sleep", std::make_shared<Type>(
        FunctionType(
            std::make_shared<Type>(PrimitiveType(PrimitiveType::Void)),
            std::vector<TypePtr>{std::make_shared<Type>(PrimitiveType(PrimitiveType::Double))}
        )
    ));
}

TypeEnvironment::TypeEnvironment(TypeEnvironmentPtr enclosing)
    : assignedType_()
    , declaredType_()
    , enclosing_(enclosing) {}

void TypeEnvironment::declareAndAssign(const std::string& name, TypePtr type) {
    declare(name, type);
    assign(name, type);
}

void TypeEnvironment::declare(const std::string& name, TypePtr type) {
    declaredType_.insert({name, type}); // Declarations can only happen once, so use insert
}

TypePtr TypeEnvironment::declaredType(const std::string& name) {
    if (declaredType_.count(name))
        return declaredType_[name];
    if (enclosing_)
        return enclosing_->declaredType(name);
    return nullptr;
}

void TypeEnvironment::assign(const std::string& name, TypePtr type) {
    assignedType_[name] = type; // Assignments can happen more than once, so use [] operator
}

TypePtr TypeEnvironment::assignedType(const std::string& name) {
    if (assignedType_.count(name))
        return assignedType_[name];
    if (enclosing_)
        return enclosing_->assignedType(name);
    return nullptr;
}

Checker::Checker(Utils::ErrorHandler& errorHandler)
    : errorHandler_(errorHandler)
    , globals_(std::make_shared<TypeEnvironment>())
    , env_(globals_) {}

void Checker::check(const std::vector<AstStatPtr> &statements) {
    // type checking
    try {
        for (const AstStatPtr& stat : statements) {
            if (!stat) throw InternalCompilerError("[Internal Compiler Error]: nullptr statement in AST list.");
            
            checkStat(*stat);
        }
    } catch (TypeError error) {
        errorHandler_.error(error.line_, error.what());
    } catch (LogicError error) {
        errorHandler_.error(error.line_, error.what());
    } catch (InternalCompilerError error) {
        std::cerr << error.what() << std::endl;
    }
}

void Checker::checkStat(AstStat& stat) {
    stat.accept(*this);
}

TypePtr Checker::checkExpr(AstExpr& expr) {
    expr.accept(*this);
    return std::move(result_);
}

TypePtr Checker::convertAstType(AstType& type) {
    type.accept(*this);
    return std::move(result_);
}

void Checker::visitPrimitiveType(AstTypePrimitive& type) {
    switch(type.kind_) {
        case AstTypePrimitive::BOOL:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            break;
        case AstTypePrimitive::INT:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            break;
        case AstTypePrimitive::DOUBLE:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
            break;
        case AstTypePrimitive::STRING:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::String});
            break;
        case AstTypePrimitive::CHAR:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Character});
            break;
        case AstTypePrimitive::VOID:
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Void});
            break;
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Primitive Type.");
    }
}

void Checker::visitFunctionType(AstTypeFunction& type) {
    TypePtr retType = convertAstType(*type.returnType);
    std::vector<TypePtr> paramTypes;
    for (auto& paramType : type.paramTypes) {
        paramTypes.push_back(convertAstType(*paramType));
    }

    result_ = std::make_shared<Type>(FunctionType{std::move(retType), std::move(paramTypes)});
}

void Checker::visitGroupExpr(AstExprGroup& expr) {
    result_ = checkExpr(*expr.expr_);
}

void Checker::visitUnaryExpr(AstExprUnary& expr) {
    TypePtr right = checkExpr(*expr.right_);

    switch (expr.op_.type_) {
        case TokenType::BANG: {
            if (!std::holds_alternative<PrimitiveType>(right->type_))
                throw TypeError(expr.op_.line_, "Unary '!' operator is not supported for type '" + right->toString() + "'.");
            PrimitiveType pt = std::get<PrimitiveType>(right->type_);
            if (pt.type_ != PrimitiveType::Boolean)
                throw TypeError(expr.op_.line_, "Unary '!' operator is only supported for 'boolean' type.");
            
            result_ = right;
            break;
        }
        case TokenType::TILDE: {
            if (!std::holds_alternative<PrimitiveType>(right->type_))
                throw TypeError(expr.op_.line_, "Unary '~' operator is not supported for type '" + right->toString() + "'.");
            PrimitiveType pt = std::get<PrimitiveType>(right->type_);
            if (pt.type_ != PrimitiveType::Integer)
                throw TypeError(expr.op_.line_, "Unary '~' operator is only supported for 'int' type.");
            
            result_ = right;
            break;
        }
        case TokenType::MINUS: {
            if (!std::holds_alternative<PrimitiveType>(right->type_))
                throw TypeError(expr.op_.line_, "Unary '-' operator is not supported for type '" + right->toString() + "'.");
            
            PrimitiveType pt = std::get<PrimitiveType>(right->type_);
            if (pt.type_ != PrimitiveType::Integer && pt.type_ != PrimitiveType::Double)
                throw TypeError(expr.op_.line_, "Unary '~' operator is only supported for `integer` or `double` type.");
                
            result_ = right;
            break;
        }
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Unary Operator: " + expr.op_.stringifyTokenType() + ".");
            break;
    }
}
void Checker::visitBinaryExpr(AstExprBinary& expr) {
    TypePtr left = checkExpr(*expr.left_);
    TypePtr right = checkExpr(*expr.right_);

    if (!std::holds_alternative<PrimitiveType>(left->type_) || !std::holds_alternative<PrimitiveType>(right->type_))
        throw TypeError(expr.line_, "Binary operators are not supported for types '" + left->toString() + "' and '" + right->toString() + "'.");

    PrimitiveType ptLeft = std::get<PrimitiveType>(left->type_);
    PrimitiveType ptRight = std::get<PrimitiveType>(right->type_);

    if (ptLeft.type_ == PrimitiveType::Void || ptRight.type_ == PrimitiveType::Void)
        throw TypeError(expr.line_, "Binary operators are not supported for types '" + ptLeft.toString() + "' and '" + ptRight.toString() + "'.");

    switch (expr.op_.type_) {
        case TokenType::SLASH: // TODO: Division by Zero error
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' / '" + ptRight.toString() + "'.");
            break;
        case TokenType::STAR:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' * '" + ptRight.toString() + "'.");
            break;
        case TokenType::PERECENT:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' % '" + ptRight.toString() + "'.");
            break;
        case TokenType::MINUS:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' - '" + ptRight.toString() + "'.");
            break;
        case TokenType::PLUS:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::String});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' + '" + ptRight.toString() + "'.");
            break;
        case TokenType::GREATER_GREATER:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' >> '" + ptRight.toString() + "'.");
            break;
        case TokenType::LESS_LESS:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' << '" + ptRight.toString() + "'.");
            break;
        case TokenType::GREATER: // TODO: Allow comparison between int and double?
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' > '" + ptRight.toString() + "'.");
            break;
        case TokenType::GREATER_EQUAL: // TODO: Allow comparison between int and double?
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' >= '" + ptRight.toString() + "'.");
            break;
        case TokenType::LESS: // TODO: Allow comparison between int and double?
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' < '" + ptRight.toString() + "'.");
            break;
        case TokenType::LESS_EQUAL: // TODO: Allow comparison between int and double?
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' <= '" + ptRight.toString() + "'.");
            break;
        case TokenType::EQUAL_EQUAL:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Boolean && ptRight.type_ == PrimitiveType::Boolean)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::NilType && ptRight.type_ == PrimitiveType::NilType)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' == '" + ptRight.toString() + "'.");
            break;
        case TokenType::BANG_EQUAL:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Double && ptRight.type_ == PrimitiveType::Double)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::String && ptRight.type_ == PrimitiveType::String)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Character && ptRight.type_ == PrimitiveType::Character)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::Boolean && ptRight.type_ == PrimitiveType::Boolean)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else if (ptLeft.type_ == PrimitiveType::NilType && ptRight.type_ == PrimitiveType::NilType)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' != '" + ptRight.toString() + "'.");
            break;
        case TokenType::PIPE:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' | '" + ptRight.toString() + "'.");
            break;
        case TokenType::AMPERSAND:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' & '" + ptRight.toString() + "'.");
            break;
        case TokenType::CARET:
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' ^ '" + ptRight.toString() + "'.");
            break;
        case TokenType::PIPE_PIPE: // TODO: implement short circuiting
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
                result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' || '" + ptRight.toString() + "'.");
            break;
        case TokenType::AMPERSAND_AMPERSAND: // TODO: implement short circuiting
            if (ptLeft.type_ == PrimitiveType::Integer && ptRight.type_ == PrimitiveType::Integer)
            result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
            else
                throw TypeError(expr.op_.line_, "Unsupported operands for types '" + ptLeft.toString() + "' && '" + ptRight.toString() + "'.");
            break;
        default:
            throw InternalCompilerError("[Internal Compiler Error]: Unexpected Binary Operator: " + expr.op_.stringifyTokenType() + ".");
    }
}

void Checker::visitTernaryExpr(AstExprTernary& expr) {
    TypePtr conditionType = checkExpr(*expr.condition_);
    if (!std::holds_alternative<PrimitiveType>(conditionType->type_))
        throw TypeError(expr.line_, "Ternary operator condition does not support type '" + conditionType->toString() + "'. It should be a `bool` type.");
    PrimitiveType ptCondition = std::get<PrimitiveType>(conditionType->type_);
    if (ptCondition.type_ != PrimitiveType::Boolean)
        throw TypeError(expr.line_, "Ternary operator condition must be of type 'bool' not '" + ptCondition.toString() + "'.");

    TypePtr thenBranchType = checkExpr(*expr.thenBranch_);
    TypePtr elseBranchType = checkExpr(*expr.elseBranch_);

    UnionType unionType({std::move(thenBranchType), std::move(elseBranchType)});
    result_ = std::make_shared<Type>(unionType);
}

void Checker::visitLiteralNullExpr(AstExprLiteralNull& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::NilType});
}

void Checker::visitLiteralBoolExpr(AstExprLiteralBool& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Boolean});
}

void Checker::visitLiteralIntExpr(AstExprLiteralInt& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Integer});
}

void Checker::visitLiteralDoubleExpr(AstExprLiteralDouble& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Double});
}

void Checker::visitLiteralStringExpr(AstExprLiteralString& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::String});
}

void Checker::visitLiteralCharExpr(AstExprLiteralChar& expr) {
    result_ = std::make_shared<Type>(PrimitiveType{PrimitiveType::PrimitiveKind::Character});
}

void Checker::visitVariableExpr(AstExprVariable& expr) {
    TypePtr t = env_->assignedType(expr.name_.lexeme_);

    if (t == nullptr)
        throw LogicError(expr.line_, "Unitialized variable '" + expr.name_.lexeme_ + "'.");

    result_ = t;
}

void Checker::visitAssignmentExpr(AstExprAssignment& expr) {
    TypePtr declaredTy = env_->declaredType(expr.name_.lexeme_);
    if (declaredTy == nullptr)
        throw LogicError(expr.line_, "Cannot assign to undeclared variable '" + expr.name_.lexeme_ + "'.");

    TypePtr t = checkExpr(*expr.value_);
    if (!t->subtypeOf(*declaredTy))
        throw TypeError(expr.line_, "Cannot assign value of type '" + t->toString() + "' to variable '" + expr.name_.lexeme_ + "' of declared type '" + declaredTy->toString() + "'.");

    env_->assign(expr.name_.lexeme_, t);
    result_ = t;
}

void Checker::visitCallExpr(AstExprCall& expr) {
    TypePtr calleeTy = checkExpr(*expr.callee_);
    if (!std::holds_alternative<FunctionType>(calleeTy->type_))
        throw TypeError(expr.line_, "Attempted to call a non-function value of type '" + calleeTy->toString() + "'.");
    
    FunctionType fnTy = std::get<FunctionType>(calleeTy->type_);
    if (expr.args_.size() != fnTy.paramTypes_.size())
        throw TypeError(expr.line_, "Function expects " + std::to_string(fnTy.paramTypes_.size()) + " argument(s) but got " + std::to_string(expr.args_.size()) + ".");
    
    for (size_t i = 0; i < expr.args_.size(); ++i) {
        TypePtr actualArg = checkExpr(*expr.args_[i]);
        TypePtr expectedArg = fnTy.paramTypes_[i];

        if (!actualArg->subtypeOf(*expectedArg))
            throw TypeError(expr.args_[i]->line_, "Argument " + std::to_string(i + 1) + " to function expects type '" + expectedArg->toString() + "', but got type '" + actualArg->toString() + "'.");
    }

    result_ = fnTy.returnType_;
}

void Checker::visitVarDeclStat(AstStatVarDecl& stat) {
    if (env_->declaredType(stat.name_.lexeme_) != nullptr)
        throw LogicError(stat.line_, "Variable '" + stat.name_.lexeme_ + "' is already declared in this scope.");

    TypePtr declaredTy = convertAstType(*stat.type_);
    TypePtr valueTy = checkExpr(*stat.initializer_);
    if (!valueTy->subtypeOf(*declaredTy))
        throw TypeError(stat.line_,  "Cannot assign value of type '" + valueTy->toString() + "' to variable '" + stat.name_.lexeme_ + "' of declared type '" + declaredTy->toString() + "'.");

    env_->declare(stat.name_.lexeme_, declaredTy);

    if (stat.initializer_ == nullptr)
        return;

    env_->assign(stat.name_.lexeme_, valueTy);
}

void Checker::visitExpressionStat(AstStatExpression& stat) {
    checkExpr(*stat.expr_);
}

void Checker::visitIfElseStat(AstStatIfElse& stat) {
    TypePtr condTy = checkExpr(*stat.condition_);
    if (!condTy->subtypeOf(Type(PrimitiveType{PrimitiveType::Boolean})))
        throw TypeError(stat.condition_->line_, "Condition of if statement must be a 'bool' type, but got '" + condTy->toString() + "'.");

    checkStat(*stat.thenBranch_);

    if (stat.elseBranch_ != nullptr)
        checkStat(*stat.elseBranch_);
}

void Checker::visitWhileStat(AstStatWhile& stat) {
    TypePtr condTy = checkExpr(*stat.condition_);
    if (!condTy->subtypeOf(Type(PrimitiveType{PrimitiveType::Boolean})))
        throw TypeError(stat.condition_->line_, "Condition of while statement must be a 'bool' type, but got '" + condTy->toString() + "'.");

    loopDepth_++;
    checkStat(*stat.body_);
    loopDepth_--;
}

void Checker::visitForStat(AstStatFor& stat) {
    TypeEnvironmentGuard guard(env_, std::make_shared<TypeEnvironment>(env_));

    if (stat.initializer_ != nullptr)
        checkStat(*stat.initializer_);

    if (stat.condition_ != nullptr) {
        TypePtr condTy = checkExpr(*stat.condition_);
        if (!condTy->subtypeOf(Type(PrimitiveType{PrimitiveType::Boolean})))
            throw TypeError(stat.condition_->line_, "Condition of for loop must be of type 'bool', but got type '" + condTy->toString() + "'.");
    }

    if (stat.increment_ != nullptr)
        checkExpr(*stat.increment_);

    loopDepth_++;
    checkStat(*stat.body_);
    loopDepth_--;
}

void Checker::visitBreakStat(AstStatBreak& stat) {
    if (loopDepth_ == 0) throw LogicError(stat.line_, "'break' can only be used inside a loop.");
}

void Checker::visitContinueStat(AstStatContinue& stat) {
    if (loopDepth_ == 0) throw LogicError(stat.line_, "'continue' can only be used inside a loop.");
}

void Checker::visitBlockStat(AstStatBlock& stat) {
    TypeEnvironmentGuard guard(env_, std::make_shared<TypeEnvironment>(env_));

    for (auto& stat : stat.body_)
        checkStat(*stat);
}

void Checker::visitFuncDeclStat(AstStatFuncDecl& stat) {
    if (env_->declaredType(stat.name_.lexeme_) != nullptr)
        throw LogicError(stat.line_, "Function '" + stat.name_.lexeme_ + "' is already declared in this scope.");

    TypePtr returnTy = convertAstType(*stat.returnType_);

    std::vector<TypePtr> paramTypes;
    for (auto& param : stat.paramTypes_)
        paramTypes.push_back(convertAstType(*param));
    
    // Important to define the function scope before we assign function to the current env
    // bc we need to access paramTypes[i], and creating a function type
    // passes in the rvalue reference of paramTypes, so accessing paramTypes[i] after
    // creating function type would cause segfault
    TypeEnvironmentPtr fnScope = std::make_shared<TypeEnvironment>(env_);
    for (size_t i = 0; i < stat.paramTypes_.size(); ++i)
        fnScope->declareAndAssign(stat.paramNames_[i].lexeme_, paramTypes[i]);
    
    TypePtr fnTy = std::make_shared<Type>(FunctionType(returnTy, paramTypes));
    env_->declareAndAssign(stat.name_.lexeme_, fnTy);
    
    TypeEnvironmentGuard guard(env_, fnScope);
    TypePtr previousReturnTy = currFunctionRetTy_;
    currFunctionRetTy_ = returnTy;
    checkStat(*stat.body_);
    currFunctionRetTy_ = previousReturnTy;
}

void Checker::visitReturnStat(AstStatReturn& stat) {
    if (currFunctionRetTy_ == nullptr)
        throw LogicError(stat.line_, "'return' can only be used inside a function.");

    TypePtr returnValueTy = stat.value_ ? checkExpr(*stat.value_) : std::make_shared<Type>(PrimitiveType(PrimitiveType::NilType));

    if (!returnValueTy->subtypeOf(*currFunctionRetTy_))
        throw TypeError(stat.line_, "Return type '" + returnValueTy->toString() + "' does not match function return type '" + currFunctionRetTy_->toString() + "'.");
}
