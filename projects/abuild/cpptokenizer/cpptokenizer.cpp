export module abuild.cpptokenizer;

#ifndef __clang__
export import : token;
import : tokenizer;
#else
export import astl;
// clang-format off
#include "condition.cpp" //NOLINT(bugprone-suspicious-include)
#include "token.cpp" //NOLINT(bugprone-suspicious-include)
#include "tokenizer_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "preprocessor_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "define_directive.cpp" //NOLINT(bugprone-suspicious-include)
#include "preprocessor_directive.cpp" //NOLINT(bugprone-suspicious-include)
#include "tokenizer.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif

namespace abuild
{
//! Converts `source` to an ordered list of
//! supported tokens.
export [[nodiscard]] auto tokenize(std::string_view source) -> std::vector<Token>
{
    std::vector<Token> tokens;
    Tokenizer tokenizer{source};
    Token token;

    while (tokenizer.next(token))
    {
        tokens.push_back(token);
    }

    return tokens;
}
}
