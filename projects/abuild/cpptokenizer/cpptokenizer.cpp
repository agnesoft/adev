export module abuild.cpptokenizer;

#ifndef __clang__
export import : token;
import : tokenizer;
#else
export import astl;
// clang-format off
#include "if_token.cpp" //NOLINT(bugprone-suspicious-include)
#include "token.cpp" //NOLINT(bugprone-suspicious-include)
#include "tokenizer_common.cpp" //NOLINT(bugprone-suspicious-include)
#include "preprocessor_tokenizer.cpp" //NOLINT(bugprone-suspicious-include)
#include "tokenizer.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif

namespace abuild
{
//! Converts `source` to an ordered list of
//! supported tokens.
export [[nodiscard]] auto tokenize(std::string_view source) -> std::vector<Token>
{
    return Tokenizer{source}.tokenize();
}
}
