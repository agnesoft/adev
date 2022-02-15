export module abuild.cache;

#ifndef __clang__
export import : cache_impl;
#else
export import abuild.cpptokenizer;
import yamlcpp;
// clang-format off
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "header_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "project.cpp" //NOLINT(bugprone-suspicious-include)
#include "settings.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_data.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_index.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_impl.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_reader.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_writer.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
