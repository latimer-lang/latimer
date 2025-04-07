#include <latimer/ast/parser.hpp>
#include <latimer/ast/ast.hpp>
#include <latimer/utils/error_handler.hpp>
#include <memory>

Parser::Parser(std::vector<Token> tokens, Utils::ErrorHandler& errorHandler)
    : tokens_(tokens)
    , current_(0)
    , errorHandler_(errorHandler) {}

std::vector<AstStatPtr> Parser::parse() {
    std::vector<AstStatPtr> statements;
    while (!isAtEnd()) {
        AstStatPtr stat = declaration();
        if (stat)
            statements.push_back(std::move(stat));
    }

    return statements; // Note: return-value optimization (RVO) uses move semantics here. so vector unique ptr is not copied, and thus keeping things safe
}

AstExprPtr Parser::expression() {
    return assignment();
}

AstExprPtr Parser::assignment() {
    AstExprPtr expr = ternary();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        AstExprPtr value = assignment();

        if (auto varExpr = dynamic_cast<AstExprVariable*>(expr.get())) {
            Token name = varExpr->name_;
            return std::make_unique<AstExprAssignment>(varExpr->line_, name, std::move(value));
        }

        error(equals, "Invalid lvalue for an assignment.");
    }

    return expr;
}

AstExprPtr Parser::ternary() {
    AstExprPtr expr = logical();

    if (match({TokenType::QUESTION_MARK})) {
        AstExprPtr thenBranch = logical();
        consume(TokenType::COLON, "Expect ':' after then-branch of ternary expression.");
        AstExprPtr elseBranch = ternary();

        expr = std::make_unique<AstExprTernary>(expr->line_, std::move(expr), std::move(thenBranch),
                                                std::move(elseBranch));
    }

    return expr;
}

AstExprPtr Parser::logical() {
    AstExprPtr expr = bitwise();

    while (match({TokenType::PIPE_PIPE, TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        AstExprPtr right = bitwise();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::bitwise() {
    AstExprPtr expr = equality();

    while (match({TokenType::PIPE, TokenType::AMPERSAND, TokenType::CARET})) {
        Token op = previous();
        AstExprPtr right = equality();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::equality() {
    AstExprPtr expr = comparison();

    while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
        Token op = previous();
        AstExprPtr right = comparison();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::comparison() {
    AstExprPtr expr = bitshift();

    while (match(
        {TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
        Token op = previous();
        AstExprPtr right = bitshift();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::bitshift() {
    AstExprPtr expr = term();

    while (match({TokenType::GREATER_GREATER, TokenType::LESS_LESS})) {
        Token op = previous();
        AstExprPtr right = term();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::term() {
    AstExprPtr expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        AstExprPtr right = factor();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::factor() {
    AstExprPtr expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERECENT})) {
        Token op = previous();
        AstExprPtr right = unary();
        expr = std::make_unique<AstExprBinary>(expr->line_, std::move(expr), op, std::move(right));
    }

    return expr;
}

AstExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::TILDE, TokenType::MINUS})) {
        Token op = previous();
        AstExprPtr expr = unary();

        return std::make_unique<AstExprUnary>(op.line_, op, std::move(expr));
    }

    return primary();
}

AstExprPtr Parser::primary() {
    if (match({TokenType::NIL})) return std::make_unique<AstExprLiteralNull>(previous().line_);

    if (match({TokenType::CHARACTER_LIT})) {
        char value = std::get<char>(previous().literal_);
        return std::make_unique<AstExprLiteralChar>(previous().line_, value);
    }
    if (match({TokenType::STRING_LIT})) {
        std::string value = std::get<std::string>(previous().literal_);
        return std::make_unique<AstExprLiteralString>(previous().line_, value);
    }
    if (match({TokenType::INTEGER_LIT})) {
        int32_t value = std::get<int32_t>(previous().literal_);
        return std::make_unique<AstExprLiteralInt>(previous().line_, value);
    }
    if (match({TokenType::FLOAT_LIT})) {
        float value = std::get<float>(previous().literal_);
        return std::make_unique<AstExprLiteralFloat>(previous().line_, value);
    }
    if (match({TokenType::TRUE_LIT}))
        return std::make_unique<AstExprLiteralBool>(previous().line_, true);
    if (match({TokenType::FALSE_LIT}))
        return std::make_unique<AstExprLiteralBool>(previous().line_, false);

    if (match({TokenType::LEFT_PAREN})) {
        AstExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<AstExprGroup>(expr->line_, std::move(expr));
    }

    if (match({TokenType::IDENTIFIER})) {
        Token name = previous();
        return std::make_unique<AstExprVariable>(name.line_, name);
    }

    throw error(peek(), "Expect expression.");
}

AstStatPtr Parser::declaration() {
    try {
        if (match({TokenType::BOOL_TY, TokenType::INT_TY, TokenType::FLOAT_TY, TokenType::CHAR_TY, TokenType::STRING_TY}))
            return varDeclStat();
        
        return statement();
    } catch (ParseError error) {
        synchronize();
        return nullptr;
    }
}

AstStatPtr Parser::statement() {
    if (match({TokenType::PRINT}))
        return printStat();
    if (match({TokenType::IF}))
        return ifElseStat();
    if (match({TokenType::WHILE}))
        return whileStat();
    if (match({TokenType::FOR}))
        return forStat();
    if (match({TokenType::BREAK}))
        return breakStat();
    if (match({TokenType::CONTINUE}))
        return continueStat();
    if (match({TokenType::LEFT_BRACE}))
        return blockStat();

    return exprStat();
}

AstStatPtr Parser::varDeclStat() {
    Token type = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    AstExprPtr initializer = nullptr;
    if (match({TokenType::EQUAL}))
        initializer = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<AstStatVarDecl>(type.line_, type, name, std::move(initializer));
}

AstStatPtr Parser::exprStat() {
    AstExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<AstStatExpression>(expr->line_, std::move(expr));
}

AstStatPtr Parser::ifElseStat() {
    consume(TokenType::LEFT_PAREN, "Expect '(' before if condition.");
    AstExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    consume(TokenType::LEFT_BRACE, "Expect '{' to parse body of if statement.");
    AstStatPtr thenBranch = blockStat();

    AstStatPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        if (match({TokenType::IF})) { // parse `else if`
            elseBranch = ifElseStat();
        } else {
            consume(TokenType::LEFT_BRACE, "Expect '{' to begin 'else' block.");
            elseBranch = blockStat();
        }
    }

    return std::make_unique<AstStatIfElse>(condition->line_, std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

AstStatPtr Parser::whileStat() {
    consume(TokenType::LEFT_PAREN, "Expect '(' before while condition.");
    AstExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");

    consume(TokenType::LEFT_BRACE, "Expect '{' to parse body of while loop.");
    AstStatPtr body = blockStat();

    return std::make_unique<AstStatWhile>(condition->line_, std::move(condition), std::move(body));
}

AstStatPtr Parser::forStat() {
    Token forToken = previous();
    consume(TokenType::LEFT_PAREN, "Expect '(' to begin for loop clause.");

    AstStatPtr initializer;
    if (match({TokenType::SEMICOLON}))
        initializer = nullptr;
    else if (match({TokenType::BOOL_TY, TokenType::INT_TY, TokenType::FLOAT_TY, TokenType::CHAR_TY, TokenType::STRING_TY}))
        initializer = varDeclStat();
    else
        initializer = exprStat();

    AstExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON))
        condition = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    AstExprPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN))
      increment = expression();

    consume(TokenType::RIGHT_PAREN, "Expect ')' to close for loop clause.");

    consume(TokenType::LEFT_BRACE, "Expect '{' to parse body of for loop.");
    AstStatPtr body = blockStat();

    return std::make_unique<AstStatFor>(forToken.line_, std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

AstStatPtr Parser::breakStat() {
    Token breakToken = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after break statement.");

    return std::make_unique<AstStatBreak>(breakToken.line_);
}

AstStatPtr Parser::continueStat() {
    Token continueToken = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after continue statement.");

    return std::make_unique<AstStatBreak>(continueToken.line_);
}

AstStatPtr Parser::printStat() {
    AstExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after print statement.");
    return std::make_unique<AstStatPrint>(expr->line_, std::move(expr));
}

AstStatPtr Parser::blockStat() {
    Token leftBrace = previous();
    std::vector<AstStatPtr> body;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        body.push_back(declaration());
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' to terminate block statements.");
    return std::make_unique<AstStatBlock>(leftBrace.line_, std::move(body));
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }

    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type_ == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type_ == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens_.at(current_);
}

Token Parser::previous() {
    return tokens_.at(current_ - 1);
}

Token Parser::consume(TokenType type, std::string msg) {
    if (check(type)) return advance();

    throw error(peek(), msg);
}

Token Parser::consume(std::initializer_list<TokenType> types, std::string msg) {
    for (TokenType type : types) {
        if (check(type)) return advance();
    }

    throw error(peek(), msg);
}

ParseError Parser::error(const Token& token, const std::string& msg) {
    errorHandler_.error(token, msg);
    return ParseError(msg);
}

void Parser::synchronize() {
    advance(); // Skip bad token

    while (!isAtEnd()) {
        if (previous().type_ == TokenType::SEMICOLON) return;

        switch (peek().type_) {
            case TokenType::CLASS:
            case TokenType::BOOL_TY:
            case TokenType::INT_TY:
            case TokenType::FLOAT_TY:
            case TokenType::CHAR_TY:
            case TokenType::STRING_TY:
            case TokenType::VOID_TY:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}
