#include <latimer/ast/ast.hpp>

void AstTypePrimitive::accept(AstVisitor& visitor) {
    visitor.visitPrimitiveType(*this);
}

void AstTypeFunction::accept(AstVisitor& visitor) {
    visitor.visitFunctionType(*this);
}

void AstExprGroup::accept(AstVisitor& visitor) {
    visitor.visitGroupExpr(*this);
}

void AstExprUnary::accept(AstVisitor& visitor) {
    visitor.visitUnaryExpr(*this);
}

void AstExprBinary::accept(AstVisitor& visitor) {
    visitor.visitBinaryExpr(*this);
}

void AstExprTernary::accept(AstVisitor& visitor) {
    visitor.visitTernaryExpr(*this);
}

void AstExprLiteralNull::accept(AstVisitor& visitor) {
    visitor.visitLiteralNullExpr(*this);
}

void AstExprLiteralBool::accept(AstVisitor& visitor) {
    visitor.visitLiteralBoolExpr(*this);
}

void AstExprLiteralInt::accept(AstVisitor& visitor) {
    visitor.visitLiteralIntExpr(*this);
}

void AstExprLiteralDouble::accept(AstVisitor& visitor) {
    visitor.visitLiteralDoubleExpr(*this);
}

void AstExprLiteralString::accept(AstVisitor& visitor) {
    visitor.visitLiteralStringExpr(*this);
}

void AstExprLiteralChar::accept(AstVisitor& visitor) {
    visitor.visitLiteralCharExpr(*this);
}

void AstExprVariable::accept(AstVisitor& visitor) {
    visitor.visitVariableExpr(*this);
}

void AstExprAssignment::accept(AstVisitor& visitor) {
    visitor.visitAssignmentExpr(*this);
}

void AstExprCall::accept(AstVisitor& visitor) {
    visitor.visitCallExpr(*this);
}

void AstStatVarDecl::accept(AstVisitor& visitor) {
    visitor.visitVarDeclStat(*this);
}

void AstStatExpression::accept(AstVisitor& visitor) {
    visitor.visitExpressionStat(*this);
}

void AstStatIfElse::accept(AstVisitor& visitor) {
    visitor.visitIfElseStat(*this);
}

void AstStatWhile::accept(AstVisitor& visitor) {
    visitor.visitWhileStat(*this);
}

void AstStatFor::accept(AstVisitor& visitor) {
    visitor.visitForStat(*this);
}

void AstStatBreak::accept(AstVisitor &visitor) {
    visitor.visitBreakStat(*this);
}

void AstStatContinue::accept(AstVisitor &visitor) {
    visitor.visitContinueStat(*this);
}

void AstStatBlock::accept(AstVisitor& visitor) {
    visitor.visitBlockStat(*this);
}

void AstStatFuncDecl::accept(AstVisitor& visitor) {
    visitor.visitFuncDeclStat(*this);
}

void AstStatReturn::accept(AstVisitor& visitor) {
    visitor.visitReturnStat(*this);
}
