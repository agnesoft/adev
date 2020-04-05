cmake_minimum_required(VERSION 3.10)

project(ADev)

#Required for clang-tidy
add_definitions(-D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH)

# Catch2 Static Library
add_library(Catch2 STATIC projects/Catch2/Catch2.cpp projects/Catch2/include/Catch2/catch.hpp)
target_compile_features(Catch2 PUBLIC cxx_std_17)
target_include_directories(Catch2 PRIVATE projects/Catch2/include/Catch2)
target_include_directories(Catch2 PUBLIC projects/Catch2/include)

#ABuild Static Library
add_library(ABuildStatic STATIC projects/ABuild/include/ABuild.hpp projects/ABuild/File.cpp projects/ABuild/include/ABuild/File.hpp)
target_compile_features(ABuildStatic PUBLIC cxx_std_17)
target_include_directories(ABuildStatic PRIVATE projects/ABuild/include/ABuild)
target_include_directories(ABuildStatic PUBLIC projects/ABuild/include)

#ABuild
add_executable(ABuild projects/ABuild/main.cpp)
target_link_libraries(ABuild ABuildStatic)

#ABuildTest
add_executable(ABuildTest projects/ABuild/test/FileTest.cpp)
target_link_libraries(ABuildTest ABuildStatic Catch2)

install(TARGETS ABuild DESTINATION bin)
install(TARGETS ABuildTest DESTINATION bin/test)
