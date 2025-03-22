#include "ast.hpp"

void AstExprGroup::accept(AstVisitor& visitor) {
    visitor.visitGroupExpr(*this);
}

void AstExprUnary::accept(AstVisitor &visitor) {
    visitor.visitUnaryExpr(*this);
}

void AstExprBinary::accept(AstVisitor& visitor) {
    visitor.visitBinaryExpr(*this);
}

void AstExprTernary::accept(AstVisitor &visitor) {
    visitor.visitTernaryExpr(*this);
}

void AstExprLiteralNull::accept(AstVisitor &visitor) {
    visitor.visitLiteralNullExpr(*this);
}

void AstExprLiteralBool::accept(AstVisitor &visitor) {
    visitor.visitLiteralBoolExpr(*this);
}

void AstExprLiteralInt::accept(AstVisitor &visitor) {
    visitor.visitLiteralIntExpr(*this);
}

void AstExprLiteralFloat::accept(AstVisitor &visitor) {
    visitor.visitLiteralFloatExpr(*this);
}

void AstExprLiteralString::accept(AstVisitor &visitor) {
    visitor.visitLiteralStringExpr(*this);
}

void AstExprLiteralChar::accept(AstVisitor &visitor) {
    visitor.visitLiteralCharExpr(*this);
}
