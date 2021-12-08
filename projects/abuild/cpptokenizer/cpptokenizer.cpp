export module abuild.cpptokenizer;

#ifndef __clang__
export import : token;
#else
export import astl;
// clang-format off
#include "condition.cpp"
#include "token.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif

namespace abuild
{
export [[nodiscard]] auto tokenize([[maybe_unused]] const std::string &source) -> std::vector<Token>
{
    return {};
}
}
