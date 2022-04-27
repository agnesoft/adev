export module abuild.cache;

#ifndef __clang__
export import :cache_impl;
#else
export import abuild.cpptokenizer;
import yamlcpp;
// clang-format off
#include "project.cpp" //NOLINT(bugprone-suspicious-include)
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
#include "linked_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "cpp_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "header_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "executable.cpp" //NOLINT(bugprone-suspicious-include)
#include "dynamic_library.cpp" //NOLINT(bugprone-suspicious-include)
#include "static_library.cpp" //NOLINT(bugprone-suspicious-include)
#include "visibility.cpp" //NOLINT(bugprone-suspicious-include)
#include "module_partition.cpp" //NOLINT(bugprone-suspicious-include)
#include "module.cpp" //NOLINT(bugprone-suspicious-include)
#include "header.cpp" //NOLINT(bugprone-suspicious-include)
#include "header_unit.cpp" //NOLINT(bugprone-suspicious-include)
#include "settings.cpp" //NOLINT(bugprone-suspicious-include)
#include "abi.cpp" //NOLINT(bugprone-suspicious-include)
#include "flag.cpp" //NOLINT(bugprone-suspicious-include)
#include "toolchain.cpp" //NOLINT(bugprone-suspicious-include)
#include "translation_unit.cpp" //NOLINT(bugprone-suspicious-include)
#include "configuration_data.cpp" //NOLINT(bugprone-suspicious-include)
#include "configuration_index.cpp" //NOLINT(bugprone-suspicious-include)
#include "configuration.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_data.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_index.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_impl.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_reader.cpp" //NOLINT(bugprone-suspicious-include)
#include "cache_writer.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif

namespace abuild
{
export using Cache = CacheImpl;
}
