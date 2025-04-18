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
#include <latimer/interpreter/ast_interpreter.hpp>
#include <latimer/semantic_analysis/checker.hpp>

void runRepl() {
    // TODO: implement
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
    
    Lexer lexer = Lexer(buf.str(), errorHandler);
    std::vector<Token> tokens = lexer.scanTokens();
    
    Parser parser = Parser(tokens, errorHandler);
    std::vector<AstStatPtr> statements = parser.parse();
    if (errorHandler.hadError_) std::exit(65);

    Checker checker = Checker(errorHandler);
    checker.check(statements);
    if (errorHandler.hadError_) std::exit(65);
    
    AstInterpreter interpreter(errorHandler);
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
