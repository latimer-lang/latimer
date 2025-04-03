#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <latimer/ast/ast.hpp>
#include <latimer/lexical_analysis/lexer.hpp>
#include <latimer/utils/ast_printer.hpp>
#include <latimer/utils/error_handler.hpp>
#include <latimer/ast/parser.hpp>
#include <latimer/vm/ast_interpreter.hpp>

void runRepl() {
    Utils::ErrorHandler errorHandler;
    AstInterpreter interpreter(errorHandler, std::make_unique<Environment>());
    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        Lexer lexer = Lexer(input, errorHandler);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser = Parser(tokens, errorHandler);
        std::vector<AstStatPtr> statements = parser.parse();

        interpreter.interpret(statements);
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

    Utils::ErrorHandler errorHandler;
    AstInterpreter interpreter(errorHandler, std::make_unique<Environment>());

    Lexer lexer = Lexer(buf.str(), errorHandler);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser = Parser(tokens, errorHandler);
    std::vector<AstStatPtr> statements = parser.parse();
    if (errorHandler.hadError_) std::exit(65);

    interpreter.interpret(statements);
    if (errorHandler.hadRuntimeError_) std::exit(70);
}

int main(int argc, char* argv[]) { // TODO: wtf is going on with `1 < 3 : 4 ? 2`
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
