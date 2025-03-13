#include <iostream>
#include <string>

static bool hadErr = false;

void runPrompt() {
    std::string input;

    while (true) {
        std::cout << "> ";
        std::cin >> input;
        if (input.empty())
            break;

        std::cout << input << std::endl;
    }
}

int main() {
    runPrompt();

    return 0;
}
