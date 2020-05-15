cmake_minimum_required(VERSION 3.15)

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

##########
# CATCH2 #
##########
# Catch2 Static Library
add_library(Catch2 STATIC projects/Catch2/Catch2.cpp
                          projects/Catch2/include/Catch2/catch.hpp)
target_compile_features(Catch2 PUBLIC cxx_std_17)
target_include_directories(Catch2 SYSTEM PRIVATE projects/Catch2/include/Catch2)
target_include_directories(Catch2 SYSTEM PUBLIC projects/Catch2/include)

#########
# ACORE #
#########
#ACore Static Library
add_library(ACore STATIC projects/ACore/include/ACore.hpp
                         projects/ACore/include/ACore/Numeric.hpp
                         projects/ACore/Numeric.cpp)
target_compile_features(ACore PUBLIC cxx_std_17)
target_include_directories(ACore PRIVATE projects/ACore/include/ACore)
target_include_directories(ACore PUBLIC projects/ACore/include)

#ACoreTest
add_executable(ACoreTest projects/ACore/test/pch.hpp
                         projects/ACore/test/NumericTest.cpp)
target_link_libraries(ACoreTest ACore Catch2)

##########
# ABUILD #
##########
#ABuild Static Library
add_library(ABuildStatic STATIC projects/ABuild/include/ABuild.hpp
                                projects/ABuild/include/ABuild/File.hpp
                                projects/ABuild/include/ABuild/Source.hpp
                                projects/ABuild/File.cpp
                                projects/ABuild/Source.cpp)
target_compile_features(ABuildStatic PUBLIC cxx_std_17)
target_include_directories(ABuildStatic PUBLIC projects/ABuild/include)
target_include_directories(ABuildStatic PRIVATE projects/ABuild/include/ABuild)

#ABuild
add_executable(ABuild projects/ABuild/main.cpp)
target_link_libraries(ABuild ABuildStatic)

#ABuildTest
add_executable(ABuildTest projects/ABuild/test/TestFile.hpp
                          projects/ABuild/test/TestFile.cpp
                          projects/ABuild/test/FileTest.cpp
                          projects/ABuild/test/SourceTest.cpp)
target_link_libraries(ABuildTest ABuildStatic Catch2)

### INSTALLATION ###
install(TARGETS ABuild DESTINATION bin)
install(TARGETS ABuildTest DESTINATION bin/test)
