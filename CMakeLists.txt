cmake_minimum_required(VERSION 3.10)

project(ADev)

#Required for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

#Required for llvm-cov
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Catch2 Static Library
add_library(Catch2 STATIC projects/Catch2/Catch2.cpp projects/Catch2/include/Catch2/catch.hpp)
target_compile_features(Catch2 PUBLIC cxx_std_17)
target_include_directories(Catch2 SYSTEM PRIVATE projects/Catch2/include/Catch2)
target_include_directories(Catch2 SYSTEM PUBLIC projects/Catch2/include)

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
