#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "latimer/ast/ast.hpp"
#include "latimer/lexical_analysis/lexer.hpp"
#include "latimer/utils/ast_printer.hpp"
#include "latimer/utils/error_handler.hpp"

Utils::ErrorHandler errorHandler;

void run(std::string src) {
    Lexer lexer = Lexer(src, errorHandler);
    std::vector<Token> tokens = lexer.scanTokens();

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        std::cout << it->stringifyType() << " " << it->lexeme_ << " " << it->stringifyLiteral()
                  << std::endl;
    }
}

void runRepl() {
    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        run(input);
        errorHandler.hadError_ = false;
    }
}

void runFile(std::string filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file";
        std::exit(-1);
    }

    std::stringstream buf;
    buf << file.rdbuf();
    run(buf.str());

    if (errorHandler.hadError_) std::exit(65);
}

int main(int argc, char* argv[]) {

    // For testing AST printer
    Token minus(TokenType::MINUS, "-", NULL, 1);
    AstExprLiteralInt one_two_three(1, 123);
    AstExprUnary negative_one_two_three(1, minus, one_two_three);

    Token multiply(TokenType::STAR, "*", NULL, 1);

    AstExprLiteralFloat forth_five_point(1, 44.67);
    AstExprGroup paren(1, forth_five_point);

    AstExpr* expr = new AstExprBinary(1, negative_one_two_three, multiply, forth_five_point);

    AstPrinter prettyPrinter;
    std::cout << "Testing pretty printer:\n" << prettyPrinter.print(*expr) << std::endl;
    // End of testing AST printer

    switch (argc) {
        case 1:
            runRepl();
            break;
        case 2:
            runFile(std::string(argv[1]));
            break;
        default:
            std::cout << "Usage: ./latimer [file_path]" << std::endl;
            return 64;
    }

    return 0;
}
