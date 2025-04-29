#pragma once

#include <variant>
#include <vector>
#include <memory>
#include <string>

struct Type;
using TypePtr = std::shared_ptr<Type>;

struct PrimitiveType {
    enum PrimitiveKind {
        NilType,
        Boolean,
        Integer,
        Double,
        String,
        Character,
        Void,
    };

    PrimitiveKind type_;

    explicit PrimitiveType(PrimitiveKind type);

    std::string toString() const;
    bool subtypeOf(const PrimitiveType& other) const;
};

struct FunctionType {
    TypePtr returnType_;
    std::vector<TypePtr> paramTypes_;

    explicit FunctionType(TypePtr returnType, std::vector<TypePtr> paramTypes);

    std::string toString() const;
    bool subtypeOf(const FunctionType& other) const;
};

// Used for argument of print native function
struct UnionType {
    std::vector<TypePtr> options_;

    explicit UnionType(std::vector<TypePtr> options);

    std::string toString() const;
    bool subtypeOf(const UnionType& other) const;
};

using TypeVariant = std::variant<PrimitiveType, FunctionType, UnionType>;
struct Type {
    TypeVariant type_;

    explicit Type(TypeVariant type);

    std::string toString() const;
    bool subtypeOf(const Type& other) const;
};
