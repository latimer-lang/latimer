#pragma once

#include <variant>
#include <string>
#include <memory>
#include <regex>
#include <sstream>
#include <iomanip>

class AstInterpreter;

namespace Runtime {

class Callable;

// Represents all possible runtime values in Latimer
using Value = std::variant<std::monostate, bool, int64_t, double, std::string, char, std::shared_ptr<Callable>>;

class Callable {
public:
    virtual ~Callable() = default;

    virtual size_t arity() const = 0;
    virtual Runtime::Value call(int line, AstInterpreter& interpreter, const std::vector<Runtime::Value>& arguments) = 0;
    virtual std::string toString() const { return "<native fn>"; }
};    

inline std::string toString(Runtime::Value value) {
    if (std::holds_alternative<std::monostate>(value)) return "null";
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
    if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
    if (std::holds_alternative<double>(value)) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(6) << std::get<double>(value);
        std::string str = ss.str();
        str.erase(str.find_last_not_of('0') + 1);
        if (str.back() == '.') str += '0';
        return str;
    }
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    if (std::holds_alternative<char>(value)) return std::string(1, std::get<char>(value));
    if (std::holds_alternative<std::shared_ptr<Callable>>(value)) return "<function>";
    return "<unknown>";
}

} // namespace Runtime
