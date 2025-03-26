#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <latimer/ast/parser.hpp>
#include "latimer/ast/ast.hpp"
#include "latimer/lexical_analysis/lexer.hpp"
#include "latimer/utils/ast_printer.hpp"
#include "latimer/utils/error_handler.hpp"

Utils::ErrorHandler errorHandler;

void run(std::string src) {
    Lexer lexer = Lexer(src, errorHandler);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser = Parser(tokens, errorHandler);

    if (errorHandler.hadError_) return;

    std::cout << AstPrinter().print(*parser.parse()) << std::endl;
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

    // // For testing AST printer: -123 * (44.67)
    // Token minus(TokenType::MINUS, "-", NULL, 1);
    // AstExprPtr one_two_three = std::make_unique<AstExprLiteralInt>(1, 123);
    // AstExprPtr negative_one_two_three = std::make_unique<AstExprUnary>(1, minus, std::move(one_two_three));

    // Token multiply(TokenType::STAR, "*", NULL, 1);

    // AstExprPtr forth_five_point = std::make_unique<AstExprLiteralFloat>(1, 44.67);
    // AstExprPtr paren = std::make_unique<AstExprGroup>(1, std::move(forth_five_point));

    // AstExprPtr expr = std::make_unique<AstExprBinary>(1, std::move(negative_one_two_three), multiply, std::move(paren));

    // AstPrinter prettyPrinter;
    // std::cout << "Testing pretty printer:\n" << prettyPrinter.print(*expr) << std::endl;
    // // End of testing AST printer

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
