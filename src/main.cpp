#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "latimer/ast/ast.hpp"
#include "latimer/lexical_analysis/lexer.hpp"
#include "latimer/utils/ast_printer.hpp"
#include "latimer/utils/error_handler.hpp"
#include <latimer/ast/parser.hpp>

Utils::ErrorHandler errorHandler;

void run(std::string src) {
    Lexer lexer = Lexer(src, errorHandler);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser = Parser(tokens, errorHandler);
    AstExprPtr expr = parser.parse();

    if (errorHandler.hadError_) return;

    std::cout << AstPrinter().print(*expr) << std::endl;
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
