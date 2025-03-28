#include <variant>

namespace Runtime {

// Represents all possible runtime values in Latimer
using Value = std::variant<std::monostate, bool, int32_t, float, std::string, char>;

} // namespace Runtime
