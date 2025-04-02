#pragma once

#include <variant>
#include <string>

namespace Runtime {

// Represents all possible runtime values in Latimer
using Value = std::variant<std::monostate, bool, int32_t, float, std::string, char>;

inline std::string toString(Runtime::Value value) {
    if (std::holds_alternative<std::monostate>(value)) return "null";
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
    if (std::holds_alternative<int32_t>(value)) return std::to_string(std::get<int32_t>(value));
    if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    if (std::holds_alternative<char>(value)) return std::string(1, std::get<char>(value));
    return "<unknown>";
}

} // namespace Runtime
