#include <latimer/lexical_analysis/lexer.hpp>

#include <any>

#include <latimer/lexical_analysis/token.hpp>

Lexer::Lexer(std::string src, Utils::ErrorHandler& errorHandler)
    : src_(src)
    , tokens_()
    , start_(0)
    , current_(0)
    , line_(1)
    , errorHandler_(errorHandler) {
    // clang-format off
    keywords_.insert({"class",   TokenType::CLASS});
    keywords_.insert({"else",    TokenType::ELSE});
    keywords_.insert({"for",     TokenType::FOR});
    keywords_.insert({"if",      TokenType::IF});
    keywords_.insert({"null",    TokenType::NIL});
    keywords_.insert({"print",   TokenType::PRINT});
    keywords_.insert({"return",  TokenType::RETURN});
    keywords_.insert({"super",   TokenType::SUPER});
    keywords_.insert({"this",    TokenType::THIS});
    keywords_.insert({"while",   TokenType::WHILE});
    keywords_.insert({"break",   TokenType::BREAK});
    keywords_.insert({"continue",TokenType::CONTINUE});
    keywords_.insert({"true",    TokenType::TRUE_LIT});
    keywords_.insert({"false",   TokenType::FALSE_LIT});
    keywords_.insert({"bool",    TokenType::BOOL_TY});
    keywords_.insert({"int",     TokenType::INT_TY});
    keywords_.insert({"float",   TokenType::FLOAT_TY});
    keywords_.insert({"char",    TokenType::CHAR_TY});
    keywords_.insert({"string",  TokenType::STRING_TY});
    keywords_.insert({"void",    TokenType::VOID_TY});
    // clang-format on
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }

    tokens_.emplace_back(TokenType::END_OF_FILE, "", std::monostate{}, line_);
    return tokens_;
}

bool Lexer::isAtEnd() {
    return current_ >= src_.length();
}

char Lexer::advance() {
    return src_.at(current_++);
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Lexer::addToken(TokenType type, Runtime::Value literal) {
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
            case 'n': // Newline
                c = '\n';
                break;
            case 't': // Tab
                c = '\t';
                break;
            case 'r': // Carriage Return
                c = '\r';
                break;
            case 'b': // Backspace
                c = '\b';
                break;
            case 'f': // Formfeed
                c = '\f';
                break;
            case 'v': // Vertical Tab
                c = '\v';
                break;
            case '0': // Null character
                c = '\0';
                break;
            case '\'': // Single Quote
                c = '\'';
                break;
            case '"': // Double Quote
                c = '\"';
                break;
            case '\\': // Backslash
                c = '\\';
                break;
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

    addToken(TokenType::CHARACTER_LIT, c);
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
    addToken(TokenType::STRING_LIT, value);
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

    std::string value = src_.substr(start_, current_ - start_);
    if (value.find('.') != std::string::npos)
        addToken(TokenType::FLOAT_LIT, std::stof(value));
    else
        addToken(TokenType::INTEGER_LIT, std::stoi(value));
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
    if (type == keywords_.end()) {
        addToken(TokenType::IDENTIFIER);
        return;
    }

    if (type->second == TokenType::TRUE_LIT)
        addToken(type->second, true);
    else if (type->second == TokenType::FALSE_LIT)
        addToken(type->second, false);
    else
        addToken(type->second);
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
        case ':':
            addToken(TokenType::COLON);
            break;
        case '?':
            addToken(TokenType::QUESTION_MARK);
            break;
        case '*':
            addToken(TokenType::STAR);
            break;
        case '%':
            addToken(TokenType::PERECENT);
            break;
        case '^':
            addToken(TokenType::CARET);
            break;
        case '~':
            addToken(TokenType::TILDE);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL
                : match('<') ? TokenType::LESS_LESS
                : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL
                : match('>') ? TokenType::GREATER_GREATER
                : TokenType::GREATER);
            break;
        case '&':
            addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND);
            break;
        case '|':
            addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE);
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
