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

    return call();
}

AstExprPtr Parser::call() {
    AstExprPtr expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            int line = previous().line_;

            std::vector<AstExprPtr> args;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    if (args.size() >= 255) {
                        errorHandler_.error(peek(), "Function call can't have more than 254 arguments.");
                    }
                    args.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RIGHT_PAREN, "Expected ')' to close function call arguments.");

            expr = std::make_unique<AstExprCall>(line, std::move(expr), std::move(args));
        } else {
            break;
        }
    }
    
    return expr;
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
        int64_t value = std::get<int64_t>(previous().literal_);
        return std::make_unique<AstExprLiteralInt>(previous().line_, value);
    }
    if (match({TokenType::DOUBLE_LIT})) {
        double value = std::get<double>(previous().literal_);
        return std::make_unique<AstExprLiteralDouble>(previous().line_, value);
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
        if (match({TokenType::BOOL_TY, TokenType::INT_TY, TokenType::DOUBLE_TY, TokenType::CHAR_TY, TokenType::STRING_TY, TokenType::VOID_TY})) {
            Token declType = previous();
            Token declName = consume(TokenType::IDENTIFIER, "Expect variable name after declaration type.");
            
            // Parsing function declarations
            if (check(TokenType::LEFT_PAREN))
                return funcDeclStat(declType, declName);
            
            // Parsing variable declarations
            return varDeclStat(declType, declName);
        }
        
        return statement();
    } catch (ParseError error) {
        synchronize();
        return nullptr;
    }
}

AstStatPtr Parser::statement() {
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
    if (match({TokenType::RETURN}))
        return returnStat();
    if (match({TokenType::LEFT_BRACE}))
        return blockStat();

    return exprStat();
}

AstStatPtr Parser::varDeclStat() {
    Token declType = previous();
    Token declName = consume(TokenType::IDENTIFIER, "Expect variable name after declaration type.");

    return varDeclStat(declType, declName);
}

AstStatPtr Parser::varDeclStat(Token type, Token name) {
    AstExprPtr initializer = nullptr;
    if (match({TokenType::EQUAL}))
        initializer = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<AstStatVarDecl>(type.line_, type, name, std::move(initializer));
}

AstStatPtr Parser::funcDeclStat(Token type, Token name) {
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    std::vector<Token> paramTypes;
    std::vector<Token> paramNames;

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (paramTypes.size() >= 255 || paramNames.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }

            Token paramType = consume({TokenType::BOOL_TY, TokenType::INT_TY, TokenType::DOUBLE_TY, TokenType::CHAR_TY, TokenType::STRING_TY}, "Expect parameter type for argument " + std::to_string(paramTypes.size()));
            paramTypes.push_back(paramType);
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name for argument " + std::to_string(paramNames.size()));
            paramNames.push_back(paramName);
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after function parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    AstStatPtr body = blockStat();

    return std::make_unique<AstStatFuncDecl>(type.line_, type, name, std::move(paramTypes), std::move(paramNames), std::move(body));
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
    else if (match({TokenType::BOOL_TY, TokenType::INT_TY, TokenType::DOUBLE_TY, TokenType::CHAR_TY, TokenType::STRING_TY}))
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

AstStatPtr Parser::returnStat() {
    Token returnToken = previous();

    AstExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON))
        value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after return statement.");

    return std::make_unique<AstStatReturn>(returnToken.line_, std::move(value));
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

bool Parser::isAtFront() {
    return current_ == 0;
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

    Token errToken = peek();
    if (!isAtFront())
        errToken = previous();
    throw error(errToken, msg);
}

Token Parser::consume(std::initializer_list<TokenType> types, std::string msg) {
    for (TokenType type : types) {
        if (check(type)) return advance();
    }

    Token errToken = peek();
    if (!isAtFront())
        errToken = previous();
    throw error(errToken, msg);
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
            case TokenType::DOUBLE_TY:
            case TokenType::CHAR_TY:
            case TokenType::STRING_TY:
            case TokenType::VOID_TY:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}
