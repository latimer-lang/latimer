#include "latimer/lexical_analysis/lexer.hpp"

#include <any>

#include "latimer/lexical_analysis/token.hpp"

Lexer::Lexer(std::string src, Utils::ErrorHandler& errorHandler)
    : src_(src)
    , tokens_()
    , start_(0)
    , current_(0)
    , line_(1)
    , errorHandler_(errorHandler) {
    // clang-format off
    keywords_.insert({"and",    TokenType::AND});
    keywords_.insert({"class",  TokenType::CLASS});
    keywords_.insert({"else",   TokenType::ELSE});
    keywords_.insert({"false",  TokenType::FALSE_});
    keywords_.insert({"for",    TokenType::FOR});
    keywords_.insert({"fun",    TokenType::FUN});
    keywords_.insert({"if",     TokenType::IF});
    keywords_.insert({"null",   TokenType::NIL});
    keywords_.insert({"or",     TokenType::OR});
    keywords_.insert({"print",  TokenType::PRINT});
    keywords_.insert({"return", TokenType::RETURN});
    keywords_.insert({"super",  TokenType::SUPER});
    keywords_.insert({"this",   TokenType::THIS});
    keywords_.insert({"true",   TokenType::TRUE_});
    keywords_.insert({"while",  TokenType::WHILE});
    keywords_.insert({"bool",   TokenType::BOOL_TY});
    keywords_.insert({"int",    TokenType::INT_TY});
    keywords_.insert({"float",  TokenType::FLOAT_TY});
    keywords_.insert({"char",   TokenType::CHAR_TY});
    keywords_.insert({"string", TokenType::STRING_TY});
    // clang-format on
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }

    tokens_.emplace_back(TokenType::END_OF_FILE, "", NULL, line_);
    return tokens_;
}

bool Lexer::isAtEnd() {
    return current_ >= src_.length();
}

char Lexer::advance() {
    return src_.at(current_++);
}

void Lexer::addToken(TokenType type) {
    addToken(type, NULL);
}

void Lexer::addToken(TokenType type, std::any literal) {
    std::string text = src_.substr(start_, current_ - start_);
    tokens_.emplace_back(type, text, literal, line_);
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (src_.at(current_) != expected) return false;

    ++current_;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return src_.at(current_);
}

void Lexer::character() {
    if (isAtEnd()) {
        errorHandler_.error(line_, "Unterminated character literal.");
        return;
    }

    char c = advance();

    if (c == '\\') {
        if (isAtEnd()) {
            errorHandler_.error(line_, "Unterminated escape sequence in character literal.");
            return;
        }

        char esc = advance();
        switch (esc) {
            case 'n':  c = '\n'; break; // Newline
            case 't':  c = '\t'; break; // Tab
            case 'r':  c = '\r'; break; // Carriage Return
            case 'b':  c = '\b'; break; // Backspace
            case 'f':  c = '\f'; break; // Formfeed
            case 'v':  c = '\v'; break; // Vertical Tab
            case '0':  c = '\0'; break; // Null character
            case '\'': c = '\''; break; // Single Quote
            case '"':  c = '\"'; break; // Double Quote
            case '\\': c = '\\'; break; // Backslash
            default:
                errorHandler_.error(line_, std::string("Unknown escape character: \\") + esc);
                return;
        }
    }

    if (peek() != '\'') {
        errorHandler_.error(line_, "Character literal must be a single character.");
        return;
    }

    advance();

    addToken(TokenType::CHARACTER, c);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') ++line_;
        advance();
    }

    if (isAtEnd()) {
        errorHandler_.error(line_, "Unterminated string.");
        return;
    }

    advance();

    std::string value = src_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING, value);
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

char Lexer::peekNext() {
    if (current_ + 1 >= src_.length()) return '\0';
    return src_.at(current_ + 1);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();

        while (isDigit(peek())) advance();
    }

    addToken(TokenType::NUMBER, std::stoi(src_.substr(start_, current_ - start_)));
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text = src_.substr(start_, current_ - start_);
    auto type = keywords_.find(text);
    if (type != keywords_.end())
        addToken(type->second);
    else
        addToken(TokenType::IDENTIFIER);
}

void Lexer::scanToken() {
    char c = advance();

    switch (c) {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case '{':
            addToken(TokenType::LEFT_BRACE);
            break;
        case '}':
            addToken(TokenType::RIGHT_BRACE);
            break;
        case ',':
            addToken(TokenType::COMMA);
            break;
        case '.':
            addToken(TokenType::DOT);
            break;
        case '-':
            addToken(TokenType::MINUS);
            break;
        case '+':
            addToken(TokenType::PLUS);
            break;
        case ';':
            addToken(TokenType::SEMICOLON);
            break;
        case '*':
            addToken(TokenType::STAR);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '/':
            if (match('/')) // Ignore comments
                while (peek() != '\n' && !isAtEnd()) advance();
            else
                addToken(TokenType::SLASH);
            break;
        case ' ':  // Ignore whitespaces
        case '\r': // Ignore whitespaces
        case '\t': // Ignore whitespaces
            break;
        case '\n':
            ++line_;
            break;
        case '\'':
            character();
            break;
        case '"':
            string();
            break;
        default:
            if (isDigit(c))
                number();
            else if (isAlpha(c))
                identifier();
            else
                errorHandler_.error(line_, "Unexpected character.");
            break;
    }
}
