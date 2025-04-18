#include <latimer/semantic_analysis/type.hpp>

PrimitiveType::PrimitiveType(PrimitiveKind type) 
    : type_(type) {}

std::string PrimitiveType::toString() const {
    switch (type_) {
        case NilType:    return "null";
        case Boolean:    return "bool";
        case Integer:    return "int";
        case Double:     return "double";
        case String:     return "string";
        case Character:  return "char";
        case Void:       return "void";
    }
    return "<unknown>";
}

bool PrimitiveType::subtypeOf(const PrimitiveType &other) const {
    if (type_ == other.type_)
        return true;

    // null → anything
    if (type_ == NilType)
        return true;

    return false;
}

FunctionType::FunctionType(TypePtr returnType, std::vector<TypePtr> paramTypes)
    : returnType_(std::move(returnType))
    , paramTypes_(std::move(paramTypes)) {}

std::string FunctionType::toString() const  {
    std::string s = returnType_->toString() + "(";
    for (size_t i = 0; i < paramTypes_.size(); ++i) {
        s += paramTypes_[i]->toString();
        if (i != paramTypes_.size() - 1)
            s += ", ";
    }
    s += ")";
    return s;   
}

bool FunctionType::subtypeOf(const FunctionType& other) const {
    if (!returnType_->subtypeOf(*other.returnType_))
        return false;

    if (paramTypes_.size() != other.paramTypes_.size())
        return false;

    for (size_t i = 0; i < paramTypes_.size(); ++i) {
        if (!other.paramTypes_[i]->subtypeOf(*paramTypes_[i]))
            return false;
    }

    return true;
}

UnionType::UnionType(std::vector<TypePtr> options)
    : options_(std::move(options)) {}

std::string UnionType::toString() const {
    std::string result;
    for (size_t i = 0; i < options_.size(); ++i) {
        result += options_[i]->toString();
        if (i != options_.size() - 1)
            result += " | ";
    }
    return result;
}

bool UnionType::subtypeOf(const UnionType& other) const {
    // Each option in THIS union must be a subtype of some option in OTHER
    for (const auto& myOpt : options_) {
        bool found = false;
        for (const auto& otherOpt : other.options_) {
            if (myOpt->subtypeOf(*otherOpt)) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

Type::Type(TypeVariant type)
    : type_(std::move(type)) {}

std::string Type::toString() const {
    return std::visit([](const auto& t) {
        return t.toString();
    }, type_);
}

bool Type::subtypeOf(const Type& other) const {
    if (type_.index() != other.type_.index())
    {
        // allow null to be assigned to any non-function type
        if (std::holds_alternative<PrimitiveType>(type_) &&
            std::get<PrimitiveType>(type_).type_ == PrimitiveType::NilType)
            return true;

        // Special case: primitive/function is allowed to be a subtype of a union
        if (std::holds_alternative<UnionType>(other.type_)) {
            const auto& otherUnion = std::get<UnionType>(other.type_);
            for (const auto& superType : otherUnion.options_) {
                if (this->subtypeOf(*superType))
                    return true;
            }
            return false;
        }

        return false;
    }

    return std::visit([&](const auto& thisType) -> bool {
        using T = std::decay_t<decltype(thisType)>;
        return thisType.subtypeOf(std::get<T>(other.type_));
    }, type_);
}
