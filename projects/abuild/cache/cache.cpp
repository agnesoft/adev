export module abuild.cache;

#ifndef __clang__
export import : header_file;
export import : source_file;
export import : project;
#else
export import abuild.cpptokenizer;
// clang-format off
#include "file.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "source_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "header_file.cpp" //NOLINT(bugprone-suspicious-include)
#include "project.cpp" //NOLINT(bugprone-suspicious-include)
// clang-format on
#endif
