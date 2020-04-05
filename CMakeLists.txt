cmake_minimum_required(VERSION 3.10)

project(ADev)

# Catch2 Static Library
add_library(Catch2 STATIC projects/Catch2/Catch2.cpp projects/Catch2/include/Catch2/catch.hpp)
target_compile_features(Catch2 PUBLIC cxx_std_17)
target_include_directories(Catch2 PRIVATE projects/Catch2/include/Catch2)
target_include_directories(Catch2 PUBLIC projects/Catch2/include)

#ABuild
add_executable(ABuild projects/ABuild/main.cpp)
target_compile_features(ABuild PRIVATE cxx_std_17)
