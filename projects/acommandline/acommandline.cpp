export module acommandline;

export import astl;

#ifndef __clang__
export import : command_line;
#else
// clang-format off
#include "option.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder_described.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder_defined.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder_value.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder_named.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_builder.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_matcher.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_setter.cpp" //NOLINT(bugprone-suspicious-include)
#include "option_help_line.cpp" //NOLINT(bugprone-suspicious-include)
#include "printer.cpp" //NOLINT(bugprone-suspicious-include)
#include "command_line.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
