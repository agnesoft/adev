cmake_minimum_required(VERSION 3.15)

project(ADev)

#Required for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(ENABLE_PCH TRUE)

#Required for llvm-cov
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    #TODO: -Wno-microsoft-cast is a workaround for Catch2 v3.0.0-preview3 (https://github.com/catchorg/Catch2/issues/2048)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -Wno-microsoft-cast")
    set(ENABLE_PCH FALSE)
endif()

#Required for Clang/LLVM memory sanitizer
if(CMAKE_BUILD_TYPE STREQUAL "SanitizeMemory")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=memory -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -nostdinc++ -I${CMAKE_SOURCE_DIR}/llvm/build_msan/include/c++/v1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory -stdlib=libc++ -L${CMAKE_SOURCE_DIR}/llvm/build_msan/lib -lc++abi -lpthread -Wl,-rpath,${CMAKE_SOURCE_DIR}/llvm/build_msan/lib")
endif()

#Required for Clang/LLVM address sanitizer
if(CMAKE_BUILD_TYPE STREQUAL "SanitizeAddress")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
endif()

#Required for Clang/LLVM undefined behaviour sanitizer
if(CMAKE_BUILD_TYPE STREQUAL "SanitizeUB")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

##########
# CATCH2 #
##########
# Catch2 Static Library
add_subdirectory(projects/Catch2)
target_compile_features(Catch2 PUBLIC cxx_std_17)

########################
# TINY-PROCESS-LIBRARY #
########################
# tiny-process-library Static Library
add_library(tiny-process-library STATIC projects/tiny-process-library/process.cpp)

if(MSVC)
  target_sources(tiny-process-library PRIVATE projects/tiny-process-library/process_win.cpp)
  target_compile_options(tiny-process-library PRIVATE "")
else()
  target_sources(tiny-process-library PRIVATE projects/tiny-process-library/process_unix.cpp)
  target_compile_options(tiny-process-library PRIVATE -Wall -Wextra)
endif()

find_package(Threads REQUIRED)

target_compile_features(tiny-process-library PUBLIC cxx_std_17)
target_link_libraries(tiny-process-library ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(tiny-process-library SYSTEM PUBLIC projects)

#########
# ACORE #
#########
#ACore Static Library
add_library(ACore STATIC projects/ACore/include/ACore.hpp
                         projects/ACore/include/ACore/ACoreModule.hpp
                         projects/ACore/include/ACore/CommandLine.hpp
                         projects/ACore/include/ACore/CommandLineOption.hpp
                         projects/ACore/include/ACore/DataIndexMap.hpp
                         projects/ACore/include/ACore/DataIndexMapBase.hpp
                         projects/ACore/include/ACore/DataIndexMapData.hpp
                         projects/ACore/include/ACore/DataIndexMapElement.hpp
                         projects/ACore/include/ACore/DataStream.hpp
                         projects/ACore/include/ACore/DataStreamBase.hpp
                         projects/ACore/include/ACore/DataStreamBuffer.hpp
                         projects/ACore/include/ACore/Dictionary.hpp
                         projects/ACore/include/ACore/DictionaryBase.hpp
                         projects/ACore/include/ACore/DictionaryData.hpp
                         projects/ACore/include/ACore/Endianness.hpp
                         projects/ACore/include/ACore/Exception.hpp
                         projects/ACore/include/ACore/ForwardIterator.hpp
                         projects/ACore/include/ACore/Numeric.hpp
                         projects/ACore/include/ACore/Process.hpp
                         projects/ACore/include/ACore/RandomAccessIterator.hpp
                         projects/ACore/include/ACore/Reference.hpp
                         projects/ACore/include/ACore/UndoStack.hpp
                         projects/ACore/include/ACore/Variant.hpp
                         projects/ACore/CommandLine.cpp
                         projects/ACore/CommandLineOption.cpp
                         projects/ACore/Endianness.cpp
                         projects/ACore/Process.cpp)
target_compile_features(ACore PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(ACore PUBLIC /W4 /WX)
else()
    target_compile_options(ACore PUBLIC -Wall -Wextra -pedantic -Werror)
endif()
target_link_libraries(ACore tiny-process-library)
target_include_directories(ACore PRIVATE projects/ACore/include/ACore)
target_include_directories(ACore PUBLIC projects/ACore/include)

#ACoreTest
add_executable(ACoreTest projects/ACore/test/CommandLineTest.cpp
                         projects/ACore/test/CommandLineOptionTest.cpp
                         projects/ACore/test/DataIndexMapElementTest.cpp
                         projects/ACore/test/DataIndexMapTest.cpp
                         projects/ACore/test/DataStreamTest.cpp
                         projects/ACore/test/DictionaryTest.cpp
                         projects/ACore/test/EndiannessTest.cpp
                         projects/ACore/test/ExceptionTest.cpp
                         projects/ACore/test/ForwardIteratorTest.cpp
                         projects/ACore/test/NumericTest.cpp
                         projects/ACore/test/ProcessTest.cpp
                         projects/ACore/test/RandomAccessIteratorTest.cpp
                         projects/ACore/test/ReferenceTest.cpp
                         projects/ACore/test/UndoStackTest.cpp
                         projects/ACore/test/VariantTest.cpp)
if(ENABLE_PCH)
    target_precompile_headers(ACoreTest PRIVATE projects/ACore/test/pch.hpp)
endif()
target_link_libraries(ACoreTest ACore Catch2::Catch2WithMain)

#########
# AFILE #
#########
#AFile Static Library
add_library(AFile STATIC projects/AFile/include/AFile.hpp
                         projects/AFile/include/AFile/AFileModule.hpp
                         projects/AFile/include/AFile/File.hpp
                         projects/AFile/include/AFile/FileData.hpp
                         projects/AFile/include/AFile/FileRecords.hpp
                         projects/AFile/include/AFile/FileStream.hpp
                         projects/AFile/include/AFile/FileStreamBuffer.hpp
                         projects/AFile/include/AFile/PersistentDataIndexMap.hpp
                         projects/AFile/include/AFile/PersistentDataIndexMapData.hpp
                         projects/AFile/include/AFile/PersistentDictionary.hpp
                         projects/AFile/include/AFile/PersistentDictionaryData.hpp
                         projects/AFile/include/AFile/PersistentVector.hpp
                         projects/AFile/include/AFile/WAL.hpp
                         projects/AFile/File.cpp
                         projects/AFile/FileData.cpp
                         projects/AFile/FileRecords.cpp
                         projects/AFile/FileStream.cpp
                         projects/AFile/FileStreamBuffer.cpp
                         projects/AFile/PersistentDataIndexMap.cpp
                         projects/AFile/PersistentDataIndexMapData.cpp
                         projects/AFile/PersistentDictionary.cpp
                         projects/AFile/PersistentDictionaryData.cpp
                         projects/AFile/WAL.cpp)
if(ENABLE_PCH)
    target_precompile_headers(AFile PRIVATE projects/AFile/pch.hpp)
endif()
target_link_libraries(AFile ACore)
target_include_directories(AFile PRIVATE projects/AFile/include/AFile)
target_include_directories(AFile PUBLIC projects/AFile/include)

#AFileTest
add_executable(AFileTest projects/AFile/test/FileTest.cpp
                         projects/AFile/test/FileStreamTest.cpp
                         projects/AFile/test/PersistentDataIndexMapTest.cpp
                         projects/AFile/test/PersistentDictionaryTest.cpp
                         projects/AFile/test/PersistentVectorTest.cpp)
if(ENABLE_PCH)
    target_precompile_headers(AFileTest PRIVATE projects/AFile/test/pch.hpp)
endif()
target_link_libraries(AFileTest AFile Catch2::Catch2WithMain)

##########
# AGRAPH #
##########
#AGraph Static Library
add_library(AGraph STATIC projects/AGraph/include/AGraph.hpp
                          projects/AGraph/include/AGraph/AGraphModule.hpp
                          projects/AGraph/include/AGraph/BreadthFirstSearch.hpp
                          projects/AGraph/include/AGraph/Common.hpp
                          projects/AGraph/include/AGraph/DepthFirstSearch.hpp
                          projects/AGraph/include/AGraph/Edge.hpp
                          projects/AGraph/include/AGraph/Element.hpp
                          projects/AGraph/include/AGraph/Graph.hpp
                          projects/AGraph/include/AGraph/GraphBase.hpp
                          projects/AGraph/include/AGraph/GraphData.hpp
                          projects/AGraph/include/AGraph/GraphSearch.hpp
                          projects/AGraph/include/AGraph/Node.hpp
                          projects/AGraph/include/AGraph/PathSearch.hpp
                          projects/AGraph/include/AGraph/PersistentGraph.hpp
                          projects/AGraph/include/AGraph/PersistentGraphData.hpp
                          projects/AGraph/PersistentGraph.cpp
                          projects/AGraph/PersistentGraphData.cpp)
if(ENABLE_PCH)
    target_precompile_headers(AGraph PRIVATE projects/AGraph/pch.hpp)
endif()
target_link_libraries(AGraph ACore AFile)
target_include_directories(AGraph PRIVATE projects/AGraph/include/AGraph)
target_include_directories(AGraph PUBLIC projects/AGraph/include)

#AGraphTest
add_executable(AGraphTest projects/AGraph/test/BreadthFirstSearchTest.cpp
                          projects/AGraph/test/CommonTest.cpp
                          projects/AGraph/test/DepthFirstSearchTest.cpp
                          projects/AGraph/test/EdgeTest.cpp
                          projects/AGraph/test/ElementTest.cpp
                          projects/AGraph/test/GraphTest.cpp
                          projects/AGraph/test/NodeTest.cpp
                          projects/AGraph/test/PathSearchTest.cpp
                          projects/AGraph/test/PersistentGraphTest.cpp)
if(ENABLE_PCH)
    target_precompile_headers(AGraphTest PRIVATE projects/AGraph/test/pch.hpp)
endif()
target_link_libraries(AGraphTest AGraph Catch2::Catch2WithMain)

### INSTALLATION ###
install(TARGETS ACoreTest AFileTest AGraphTest DESTINATION bin/test)

#Licenses
install(FILES LICENSE DESTINATION bin/LICENSE)
install(FILES projects/Catch2/LICENSE.txt DESTINATION bin/LICENSE/Catch2)
install(FILES projects/tiny-process-library/LICENSE DESTINATION bin/LICENSE/tiny-process-library)
