#ifndef ASTL_ASTL_HPP
#define ASTL_ASTL_HPP

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpedantic"
#    ifdef _WIN32
#        include <intrin.h>
#    endif
#endif

// Needs to go first to prevent ICE (MSVC 17.2.0, 16. 5. 2022)
#if __has_include(<fstream>)
#    include <fstream>
#else
#    pragma message("<fstream> not available")
#endif

// Concepts
#if __has_include(<concepts>)
#    include <concepts> //C++20
#else
#    pragma message("<concepts> not available (C++20)")
#endif

// Coroutines
#if __has_include(<coroutine>)
#    include <coroutine> //C++20
#else
#    pragma message("<coroutine> not available (C++20)")
#endif

// Utilities
#if __has_include(<cstdlib>)
#    include <cstdlib>
#else
#    pragma message("<cstdlib> not available")
#endif

#if __has_include(<csignal>)
#    include <csignal>
#else
#    pragma message("<csignal> not available")
#endif

#if __has_include(<csetjmp>)
#    include <csetjmp>
#else
#    pragma message("<csetjmp> not available")
#endif

#if __has_include(<cstdarg>)
#    include <cstdarg>
#else
#    pragma message("<cstdarg> not available")
#endif

#if __has_include(<typeinfo>)
#    include <typeinfo>
#else
#    pragma message("<typeinfo> not available")
#endif

#if __has_include(<typeindex>)
#    include <typeindex> //C++11
#else
#    pragma message("<typeindex> not available (C++11)")
#endif

#if __has_include(<type_traits>)
#    include <type_traits> //C++11
#else
#    pragma message("<type_traits> not available (C++11)")
#endif

#if __has_include(<bitset>)
#    include <bitset>
#else
#    pragma message("<bitset> not available")
#endif

#if __has_include(<functional>)
#    include <functional>
#else
#    pragma message("<functional> not available")
#endif

#if __has_include(<utility>)
#    include <utility>
#else
#    pragma message("<utility> not available")
#endif

#if __has_include(<ctime>)
#    include <ctime>
#else
#    pragma message("<ctime> not available")
#endif

#if __has_include(<chrono>)
#    include <chrono> //C++11
#else
#    pragma message("<chrono> not available (C++11)")
#endif

#if __has_include(<cstddef>)
#    include <cstddef>
#else
#    pragma message("<cstddef> not available")
#endif

#if __has_include(<initializer_list>)
#    include <initializer_list> //C++11
#else
#    pragma message("<initializer_list> not available (C++11)")
#endif

#if __has_include(<tuple>)
#    include <tuple> //C++11
#else
#    pragma message("<tuple> not available (C++11)")
#endif

#if __has_include(<any>)
#    include <any> //C++17
#else
#    pragma message("<any> not available (C++17)")
#endif

#if __has_include(<optional>)
#    include <optional> //C++17
#else
#    pragma message("<optional> not available (C++17)")
#endif

#if __has_include(<variant>)
#    include <variant> //C++17
#else
#    pragma message("<variant> not available (C++17)")
#endif

#if __has_include(<compare>)
#    include <compare> //C++20
#else
#    pragma message("<compare> not available (C++20)")
#endif

#if __has_include(<version>)
#    include <version> //C++20
#else
#    pragma message("<version> not available (C++20)")
#endif

#if __has_include(<source_location>) && !defined(__clang__)
#    include <source_location> //C++20
#else
#    pragma message("<source_location> not available (C++20)")
#endif

// Memory
#if __has_include(<new>)
#    include <new>
#else
#    pragma message("<new> not available")
#endif

#if __has_include(<memory>)
#    include <memory>
#else
#    pragma message("<memory> not available")
#endif

#if __has_include(<scoped_allocator>)
#    include <scoped_allocator> //C++11
#else
#    pragma message("<scoped_allocator> not available (C++11)")
#endif

#if __has_include(<memory_resource>)
#    include <memory_resource> //C++17
#else
#    pragma message("<memory_resource> not available (C++17)")
#endif

// Limits
#if __has_include(<climits>)
#    include <climits>
#else
#    pragma message("<climits> not available")
#endif

#if __has_include(<cfloat>)
#    include <cfloat>
#else
#    pragma message("<cfloat> not available")
#endif

#if __has_include(<cstdint>)
#    include <cstdint> //C++11
#else
#    pragma message("<cstdint> not available (C++11)")
#endif

#if __has_include(<cinttypes>)
#    include <cinttypes> //C++11
#else
#    pragma message("<cinttypes> not available (C++11)")
#endif

#if __has_include(<limits>)
#    include <limits>
#else
#    pragma message("<limits> not available")
#endif

// Error
#if __has_include(<exception>)
#    include <exception>
#else
#    pragma message("<exception> not available")
#endif

#if __has_include(<stdexcept>)
#    include <stdexcept>
#else
#    pragma message("<stdexcept> not available")
#endif

#if __has_include(<cassert>)
#    include <cassert>
#else
#    pragma message("<cassert> not available")
#endif

#if __has_include(<system_error>)
#    include <system_error> //C++11
#else
#    pragma message("<system_error> not available (C++11)")
#endif

#if __has_include(<cerrno>)
#    include <cerrno>
#else
#    pragma message("<cerrno> not available")
#endif

// Strings
#if __has_include(<cctype>)
#    include <cctype>
#else
#    pragma message("<cctype> not available")
#endif

#if __has_include(<cwctype>)
#    include <cwctype>
#else
#    pragma message("<cwctype> not available")
#endif

#if __has_include(<cstring>)
#    include <cstring>
#else
#    pragma message("<cstring> not available")
#endif

#if __has_include(<cwchar>)
#    include <cwchar>
#else
#    pragma message("<cwchar> not available")
#endif

#if __has_include(<cuchar>)
#    include <cuchar> //C++11
#else
#    pragma message("<cuchar> not available (C++11)")
#endif

#if __has_include(<string>)
#    include <string>
#else
#    pragma message("<string> not available")
#endif

#if __has_include(<string_view>)
#    include <string_view> //C++17
#else
#    pragma message("<string_view> not available (C++17)")
#endif

#if __has_include(<charconv>)
#    include <charconv> //C++17
#else
#    pragma message("<charconv> not available (C++17)")
#endif

#if __has_include(<format>)
#    include <format> //C++20
#else
#    pragma message("<format> not available (C++20)")
#endif

// Containers
#if __has_include(<array>)
#    include <array> //C++11
#else
#    pragma message("<array> not available (C++11)")
#endif

#if __has_include(<vector>)
#    include <vector>
#else
#    pragma message("<vector> not available")
#endif

#if __has_include(<deque>)
#    include <deque>
#else
#    pragma message("<deque> not available")
#endif

#if __has_include(<list>)
#    include <list>
#else
#    pragma message("<list> not available")
#endif

#if __has_include(<forward_list>)
#    include <forward_list> //C++11
#else
#    pragma message("<forward_list> not available (C++11)")
#endif

#if __has_include(<set>)
#    include <set>
#else
#    pragma message("<set> not available")
#endif

#if __has_include(<map>)
#    include <map>
#else
#    pragma message("<map> not available")
#endif

#if __has_include(<unordered_set>)
#    include <unordered_set> //C++11
#else
#    pragma message("<unordered_set> not available (C++11)")
#endif

#if __has_include(<unordered_map>)
#    include <unordered_map> //C++11
#else
#    pragma message("<unordered_map> not available (C++11)")
#endif

#if __has_include(<stack>)
#    include <stack>
#else
#    pragma message("<stack> not available")
#endif

#if __has_include(<queue>)
#    include <queue>
#else
#    pragma message("<queue> not available")
#endif

#if __has_include(<span>)
#    include <span> //C++20
#else
#    pragma message("<span> not available (C++20)")
#endif

// Iterators
#if __has_include(<iterator>)
#    include <iterator>
#else
#    pragma message("<iterator> not available")
#endif

// Ranges
#if __has_include(<ranges>)
#    include <ranges> //C++20
#else
#    pragma message("<ranges> not available (C++20)")
#endif

// Algorithms
#if __has_include(<algorithm>)
#    include <algorithm>
#else
#    pragma message("<algorithm> not available")
#endif

#if __has_include(<execution>)
#    include <execution> //C++17
#else
#    pragma message("<execution> not available (C++17)")
#endif

// Numerics
#if __has_include(<cmath>)
#    include <cmath>
#else
#    pragma message("<cmath> not available")
#endif

#if __has_include(<complex>)
#    include <complex>
#else
#    pragma message("<complex> not available")
#endif

#if __has_include(<valarray>)
#    include <valarray>
#else
#    pragma message("<valarray> not available")
#endif

#if __has_include(<random>)
#    include <random> //C++11
#else
#    pragma message("<random> not available (C++11)")
#endif

#if __has_include(<numeric>)
#    include <numeric>
#else
#    pragma message("<numeric> not available")
#endif

#if __has_include(<ratio>)
#    include <ratio> //C++11
#else
#    pragma message("<ratio> not available (C++11)")
#endif

#if __has_include(<cfenv>)
#    include <cfenv> //C++11
#else
#    pragma message("<cfenv> not available (C++11)")
#endif

#if __has_include(<bit>)
#    include <bit> //C++20
#else
#    pragma message("<bit> not available (C++20)")
#endif

#if __has_include(<numbers>)
#    include <numbers> //C++20
#else
#    pragma message("<numbers> not available (C++20)")
#endif

// Localization
#if __has_include(<locale>)
#    include <locale>
#else
#    pragma message("<locale> not available")
#endif

#if __has_include(<clocale>)
#    include <clocale>
#else
#    pragma message("<clocale> not available")
#endif

// IO
#if __has_include(<iosfwd>)
#    include <iosfwd>
#else
#    pragma message("<iosfwd> not available")
#endif

#if __has_include(<ios>)
#    include <ios>
#else
#    pragma message("<ios> not available")
#endif

#if __has_include(<istream>)
#    include <istream>
#else
#    pragma message("<istream> not available")
#endif

#if __has_include(<ostream>)
#    include <ostream>
#else
#    pragma message("<ostream> not available")
#endif

#if __has_include(<iostream>)
#    include <iostream>
#else
#    pragma message("<iostream> not available")
#endif

#if __has_include(<sstream>)
#    include <sstream>
#else
#    pragma message("<sstream> not available")
#endif

#if __has_include(<syncstream>)
#    include <syncstream> //C++20
#else
#    pragma message("<syncstream> not available (C++20)")
#endif

#if __has_include(<iomanip>)
#    include <iomanip>
#else
#    pragma message("<iomanip> not available")
#endif

#if __has_include(<streambuf>)
#    include <streambuf>
#else
#    pragma message("<streambuf> not available")
#endif

#if __has_include(<cstdio>)
#    include <cstdio>
#else
#    pragma message("<cstdio> not available")
#endif

// Filesystem
#if __has_include(<filesystem>)
#    include <filesystem> //C++17
#else
#    pragma message("<filesystem> not available (C++17)")
#endif

// Regexp
#if __has_include(<regex>)
#    include <regex> //C++11
#else
#    pragma message("<regex> not available (C++11)")
#endif

// Atomic
#if __has_include(<atomic>)
#    include <atomic> //C++11
#else
#    pragma message("<atomic> not available (C++11)")
#endif

// Thread
#if __has_include(<thread>)
#    include <thread> //C++11
#else
#    pragma message("<thread> not available (C++11)")
#endif

#if __has_include(<stop_token>)
#    include <stop_token> //C++20
#else
#    pragma message("<stop_token> not available (C++20)")
#endif

#if __has_include(<mutex>)
#    include <mutex> //C++11
#else
#    pragma message("<mutex> not available (C++11)")
#endif

#if __has_include(<shared_mutex>)
#    include <shared_mutex> //C++14
#else
#    pragma message("<shared_mutex> not available (C++14)")
#endif

#if __has_include(<future>)
#    include <future> //C++11
#else
#    pragma message("<future> not available (C++11)")
#endif

#if __has_include(<condition_variable>)
#    include <condition_variable> //C++11
#else
#    pragma message("<condition_variable> not available (C++11)")
#endif

#if __has_include(<semaphore>)
#    include <semaphore> //C++20
#else
#    pragma message("<semaphore> not available (C++20)")
#endif

#if __has_include(<latch>)
#    include <latch> //C++20
#else
#    pragma message("<latch> not available (C++20)")
#endif

#if __has_include(<barrier>)
#    include <barrier> //C++20
#else
#    pragma message("<barrier> not available (C++20)")
#endif

#ifdef __clang__
#    pragma clang diagnostic pop
#endif

#endif
