#pragma once

#include <chrono>
#include <memory>
#include <thread>

#include <latimer/interpreter/value.hpp>
#include <latimer/utils/error_handler.hpp>
#include <latimer/utils/macros.hpp>

class NativePrint : public Runtime::Callable {
public:
    size_t arity() const override {
        return 255;
    }

    Runtime::Value call(UNUSED int line, UNUSED AstInterpreter& interpreter, const std::vector<Runtime::Value>& arguments) override {
        for (size_t i = 0; i < arguments.size(); ++i) {
            std::cout << Runtime::toString(arguments[i]);
            if (i != arguments.size() - 1)
                std::cout << " ";
        }
        std::cout << std::endl;

        return std::monostate();
    }

    std::string toString() const override {
        return "<native fn print>";
    }
};

class NativeClock : public Runtime::Callable {
public:
    size_t arity() const override {
        return 0;
    }

    Runtime::Value call(UNUSED int line, UNUSED AstInterpreter& interpreter, UNUSED const std::vector<Runtime::Value>& arguments) override {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
        
        return static_cast<double>(ms) / 1000.0;
    }

    std::string toString() const override {
        return "<native fn clock>";
    }
};

class NativeSleep : public Runtime::Callable {
public:
    size_t arity() const override {
        return 1;
    }

    Runtime::Value call(int line, UNUSED AstInterpreter& interpreter, const std::vector<Runtime::Value>& arguments) override {
        using namespace std::chrono;

        const Runtime::Value& durationVal = arguments.at(0);
        if (!std::holds_alternative<double>(durationVal)) {
            throw RuntimeError(line, "sleep() expects a double (number of seconds).");
        }

        double seconds = std::get<double>(durationVal);
        if (seconds < 0.0) {
            throw RuntimeError(line, "sleep() duration must be non-negative.");
        }

        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration<double>(seconds)
        );
        std::this_thread::sleep_for(duration);

        return std::monostate();
    }

    std::string toString() const override {
        return "<native fn sleep>";
    }
};
