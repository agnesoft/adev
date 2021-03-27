//GENERATED FILE. DO NOT MODIFY.

#ifndef ADEV_ASTL_ASTL_HPP
#define ADEV_ASTL_ASTL_HPP

//Concepts
#if __has_include(<concepts>)
#    include <concepts> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<concepts> not available (C++20)")
#    else
#        warning "<concepts> not available (C++20)"
#    endif
#endif

//Coroutines
#if __has_include(<coroutines>)
#    include <coroutines> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<coroutines> not available (C++20)")
#    else
#        warning "<coroutines> not available (C++20)"
#    endif
#endif

//Utilities
#if __has_include(<cstdlib>)
#    include <cstdlib>
#else
#    ifdef _MSC_VER
#        pragma message("<cstdlib> not available")
#    else
#        warning "<cstdlib> not available"
#    endif
#endif

#if __has_include(<csignal>)
#    include <csignal>
#else
#    ifdef _MSC_VER
#        pragma message("<csignal> not available")
#    else
#        warning "<csignal> not available"
#    endif
#endif

#if __has_include(<csetjmp>)
#    include <csetjmp>
#else
#    ifdef _MSC_VER
#        pragma message("<csetjmp> not available")
#    else
#        warning "<csetjmp> not available"
#    endif
#endif

#if __has_include(<cstdarg>)
#    include <cstdarg>
#else
#    ifdef _MSC_VER
#        pragma message("<cstdarg> not available")
#    else
#        warning "<cstdarg> not available"
#    endif
#endif

#if __has_include(<typeinfo>)
#    include <typeinfo>
#else
#    ifdef _MSC_VER
#        pragma message("<typeinfo> not available")
#    else
#        warning "<typeinfo> not available"
#    endif
#endif

#if __has_include(<typeindex>)
#    include <typeindex> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<typeindex> not available (C++11)")
#    else
#        warning "<typeindex> not available (C++11)"
#    endif
#endif

#if __has_include(<type_traits>)
#    include <type_traits> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<type_traits> not available (C++11)")
#    else
#        warning "<type_traits> not available (C++11)"
#    endif
#endif

#if __has_include(<bitset>)
#    include <bitset>
#else
#    ifdef _MSC_VER
#        pragma message("<bitset> not available")
#    else
#        warning "<bitset> not available"
#    endif
#endif

#if __has_include(<functional>)
#    include <functional>
#else
#    ifdef _MSC_VER
#        pragma message("<functional> not available")
#    else
#        warning "<functional> not available"
#    endif
#endif

#if __has_include(<utility>)
#    include <utility>
#else
#    ifdef _MSC_VER
#        pragma message("<utility> not available")
#    else
#        warning "<utility> not available"
#    endif
#endif

#if __has_include(<ctime>)
#    include <ctime>
#else
#    ifdef _MSC_VER
#        pragma message("<ctime> not available")
#    else
#        warning "<ctime> not available"
#    endif
#endif

#if __has_include(<chrono>)
#    include <chrono> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<chrono> not available (C++11)")
#    else
#        warning "<chrono> not available (C++11)"
#    endif
#endif

#if __has_include(<cstddef>)
#    include <cstddef>
#else
#    ifdef _MSC_VER
#        pragma message("<cstddef> not available")
#    else
#        warning "<cstddef> not available"
#    endif
#endif

#if __has_include(<initializer_list>)
#    include <initializer_list> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<initializer_list> not available (C++11)")
#    else
#        warning "<initializer_list> not available (C++11)"
#    endif
#endif

#if __has_include(<tuple>)
#    include <tuple> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<tuple> not available (C++11)")
#    else
#        warning "<tuple> not available (C++11)"
#    endif
#endif

#if __has_include(<any>)
#    include <any> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<any> not available (C++17)")
#    else
#        warning "<any> not available (C++17)"
#    endif
#endif

#if __has_include(<optional>)
#    include <optional> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<optional> not available (C++17)")
#    else
#        warning "<optional> not available (C++17)"
#    endif
#endif

#if __has_include(<variant>)
#    include <variant> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<variant> not available (C++17)")
#    else
#        warning "<variant> not available (C++17)"
#    endif
#endif

#if __has_include(<compare>)
#    include <compare> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<compare> not available (C++20)")
#    else
#        warning "<compare> not available (C++20)"
#    endif
#endif

#if __has_include(<version>)
#    include <version> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<version> not available (C++20)")
#    else
#        warning "<version> not available (C++20)"
#    endif
#endif

#if __has_include(<source_location>)
#    include <source_location> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<source_location> not available (C++20)")
#    else
#        warning "<source_location> not available (C++20)"
#    endif
#endif

//Memory
#if __has_include(<new>)
#    include <new>
#else
#    ifdef _MSC_VER
#        pragma message("<new> not available")
#    else
#        warning "<new> not available"
#    endif
#endif

#if __has_include(<memory>)
#    include <memory>
#else
#    ifdef _MSC_VER
#        pragma message("<memory> not available")
#    else
#        warning "<memory> not available"
#    endif
#endif

#if __has_include(<scoped_allocator>)
#    include <scoped_allocator> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<scoped_allocator> not available (C++11)")
#    else
#        warning "<scoped_allocator> not available (C++11)"
#    endif
#endif

#if __has_include(<memory_resource>)
#    include <memory_resource> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<memory_resource> not available (C++17)")
#    else
#        warning "<memory_resource> not available (C++17)"
#    endif
#endif

//Limits
#if __has_include(<climits>)
#    include <climits>
#else
#    ifdef _MSC_VER
#        pragma message("<climits> not available")
#    else
#        warning "<climits> not available"
#    endif
#endif

#if __has_include(<cfloat>)
#    include <cfloat>
#else
#    ifdef _MSC_VER
#        pragma message("<cfloat> not available")
#    else
#        warning "<cfloat> not available"
#    endif
#endif

#if __has_include(<cstdint>)
#    include <cstdint> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<cstdint> not available (C++11)")
#    else
#        warning "<cstdint> not available (C++11)"
#    endif
#endif

#if __has_include(<cinttypes>)
#    include <cinttypes> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<cinttypes> not available (C++11)")
#    else
#        warning "<cinttypes> not available (C++11)"
#    endif
#endif

#if __has_include(<limits>)
#    include <limits>
#else
#    ifdef _MSC_VER
#        pragma message("<limits> not available")
#    else
#        warning "<limits> not available"
#    endif
#endif

//Error
#if __has_include(<exception>)
#    include <exception>
#else
#    ifdef _MSC_VER
#        pragma message("<exception> not available")
#    else
#        warning "<exception> not available"
#    endif
#endif

#if __has_include(<stdexcept>)
#    include <stdexcept>
#else
#    ifdef _MSC_VER
#        pragma message("<stdexcept> not available")
#    else
#        warning "<stdexcept> not available"
#    endif
#endif

#if __has_include(<cassert>)
#    include <cassert>
#else
#    ifdef _MSC_VER
#        pragma message("<cassert> not available")
#    else
#        warning "<cassert> not available"
#    endif
#endif

#if __has_include(<system_error>)
#    include <system_error> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<system_error> not available (C++11)")
#    else
#        warning "<system_error> not available (C++11)"
#    endif
#endif

#if __has_include(<cerrno>)
#    include <cerrno>
#else
#    ifdef _MSC_VER
#        pragma message("<cerrno> not available")
#    else
#        warning "<cerrno> not available"
#    endif
#endif

//Strings
#if __has_include(<cctype>)
#    include <cctype>
#else
#    ifdef _MSC_VER
#        pragma message("<cctype> not available")
#    else
#        warning "<cctype> not available"
#    endif
#endif

#if __has_include(<cwctype>)
#    include <cwctype>
#else
#    ifdef _MSC_VER
#        pragma message("<cwctype> not available")
#    else
#        warning "<cwctype> not available"
#    endif
#endif

#if __has_include(<cstring>)
#    include <cstring>
#else
#    ifdef _MSC_VER
#        pragma message("<cstring> not available")
#    else
#        warning "<cstring> not available"
#    endif
#endif

#if __has_include(<cwchar>)
#    include <cwchar>
#else
#    ifdef _MSC_VER
#        pragma message("<cwchar> not available")
#    else
#        warning "<cwchar> not available"
#    endif
#endif

#if __has_include(<cuchar>)
#    include <cuchar> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<cuchar> not available (C++11)")
#    else
#        warning "<cuchar> not available (C++11)"
#    endif
#endif

#if __has_include(<string>)
#    include <string>
#else
#    ifdef _MSC_VER
#        pragma message("<string> not available")
#    else
#        warning "<string> not available"
#    endif
#endif

#if __has_include(<string_view>)
#    include <string_view> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<string_view> not available (C++17)")
#    else
#        warning "<string_view> not available (C++17)"
#    endif
#endif

#if __has_include(<charconv>)
#    include <charconv> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<charconv> not available (C++17)")
#    else
#        warning "<charconv> not available (C++17)"
#    endif
#endif

#if __has_include(<format>)
#    include <format> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<format> not available (C++20)")
#    else
#        warning "<format> not available (C++20)"
#    endif
#endif

//Containers
#if __has_include(<array>)
#    include <array> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<array> not available (C++11)")
#    else
#        warning "<array> not available (C++11)"
#    endif
#endif

#if __has_include(<vector>)
#    include <vector>
#else
#    ifdef _MSC_VER
#        pragma message("<vector> not available")
#    else
#        warning "<vector> not available"
#    endif
#endif

#if __has_include(<deque>)
#    include <deque>
#else
#    ifdef _MSC_VER
#        pragma message("<deque> not available")
#    else
#        warning "<deque> not available"
#    endif
#endif

#if __has_include(<list>)
#    include <list>
#else
#    ifdef _MSC_VER
#        pragma message("<list> not available")
#    else
#        warning "<list> not available"
#    endif
#endif

#if __has_include(<forward_list>)
#    include <forward_list> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<forward_list> not available (C++11)")
#    else
#        warning "<forward_list> not available (C++11)"
#    endif
#endif

#if __has_include(<set>)
#    include <set>
#else
#    ifdef _MSC_VER
#        pragma message("<set> not available")
#    else
#        warning "<set> not available"
#    endif
#endif

#if __has_include(<map>)
#    include <map>
#else
#    ifdef _MSC_VER
#        pragma message("<map> not available")
#    else
#        warning "<map> not available"
#    endif
#endif

#if __has_include(<unordered_set>)
#    include <unordered_set> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<unordered_set> not available (C++11)")
#    else
#        warning "<unordered_set> not available (C++11)"
#    endif
#endif

#if __has_include(<unordered_map>)
#    include <unordered_map> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<unordered_map> not available (C++11)")
#    else
#        warning "<unordered_map> not available (C++11)"
#    endif
#endif

#if __has_include(<stack>)
#    include <stack>
#else
#    ifdef _MSC_VER
#        pragma message("<stack> not available")
#    else
#        warning "<stack> not available"
#    endif
#endif

#if __has_include(<queue>)
#    include <queue>
#else
#    ifdef _MSC_VER
#        pragma message("<queue> not available")
#    else
#        warning "<queue> not available"
#    endif
#endif

#if __has_include(<span>)
#    include <span> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<span> not available (C++20)")
#    else
#        warning "<span> not available (C++20)"
#    endif
#endif

//Iterators
#if __has_include(<iterator>)
#    include <iterator>
#else
#    ifdef _MSC_VER
#        pragma message("<iterator> not available")
#    else
#        warning "<iterator> not available"
#    endif
#endif

//Ranges
#if __has_include(<ranges>)
#    include <ranges> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<ranges> not available (C++20)")
#    else
#        warning "<ranges> not available (C++20)"
#    endif
#endif

//Algorithms
#if __has_include(<algorithm>)
#    include <algorithm>
#else
#    ifdef _MSC_VER
#        pragma message("<algorithm> not available")
#    else
#        warning "<algorithm> not available"
#    endif
#endif

#if __has_include(<execution>)
#    include <execution> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<execution> not available (C++17)")
#    else
#        warning "<execution> not available (C++17)"
#    endif
#endif

//Numerics
#if __has_include(<cmath>)
#    include <cmath>
#else
#    ifdef _MSC_VER
#        pragma message("<cmath> not available")
#    else
#        warning "<cmath> not available"
#    endif
#endif

#if __has_include(<complex>)
#    include <complex>
#else
#    ifdef _MSC_VER
#        pragma message("<complex> not available")
#    else
#        warning "<complex> not available"
#    endif
#endif

#if __has_include(<valarray>)
#    include <valarray>
#else
#    ifdef _MSC_VER
#        pragma message("<valarray> not available")
#    else
#        warning "<valarray> not available"
#    endif
#endif

#if __has_include(<random>)
#    include <random> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<random> not available (C++11)")
#    else
#        warning "<random> not available (C++11)"
#    endif
#endif

#if __has_include(<numeric>)
#    include <numeric>
#else
#    ifdef _MSC_VER
#        pragma message("<numeric> not available")
#    else
#        warning "<numeric> not available"
#    endif
#endif

#if __has_include(<ratio>)
#    include <ratio> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<ratio> not available (C++11)")
#    else
#        warning "<ratio> not available (C++11)"
#    endif
#endif

#if __has_include(<cfenv>)
#    include <cfenv> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<cfenv> not available (C++11)")
#    else
#        warning "<cfenv> not available (C++11)"
#    endif
#endif

#if __has_include(<bit>)
#    include <bit> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<bit> not available (C++20)")
#    else
#        warning "<bit> not available (C++20)"
#    endif
#endif

#if __has_include(<numbers>)
#    include <numbers> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<numbers> not available (C++20)")
#    else
#        warning "<numbers> not available (C++20)"
#    endif
#endif

//Localization
#if __has_include(<locale>)
#    include <locale>
#else
#    ifdef _MSC_VER
#        pragma message("<locale> not available")
#    else
#        warning "<locale> not available"
#    endif
#endif

#if __has_include(<clocale>)
#    include <clocale>
#else
#    ifdef _MSC_VER
#        pragma message("<clocale> not available")
#    else
#        warning "<clocale> not available"
#    endif
#endif

//IO
#if __has_include(<iosfwd>)
#    include <iosfwd>
#else
#    ifdef _MSC_VER
#        pragma message("<iosfwd> not available")
#    else
#        warning "<iosfwd> not available"
#    endif
#endif

#if __has_include(<ios>)
#    include <ios>
#else
#    ifdef _MSC_VER
#        pragma message("<ios> not available")
#    else
#        warning "<ios> not available"
#    endif
#endif

#if __has_include(<istream>)
#    include <istream>
#else
#    ifdef _MSC_VER
#        pragma message("<istream> not available")
#    else
#        warning "<istream> not available"
#    endif
#endif

#if __has_include(<ostream>)
#    include <ostream>
#else
#    ifdef _MSC_VER
#        pragma message("<ostream> not available")
#    else
#        warning "<ostream> not available"
#    endif
#endif

#if __has_include(<iostream>)
#    include <iostream>
#else
#    ifdef _MSC_VER
#        pragma message("<iostream> not available")
#    else
#        warning "<iostream> not available"
#    endif
#endif

#if __has_include(<fstream>)
#    include <fstream>
#else
#    ifdef _MSC_VER
#        pragma message("<fstream> not available")
#    else
#        warning "<fstream> not available"
#    endif
#endif

#if __has_include(<sstream>)
#    include <sstream>
#else
#    ifdef _MSC_VER
#        pragma message("<sstream> not available")
#    else
#        warning "<sstream> not available"
#    endif
#endif

#if __has_include(<syncstream>)
#    include <syncstream> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<syncstream> not available (C++20)")
#    else
#        warning "<syncstream> not available (C++20)"
#    endif
#endif

#if __has_include(<iomanip>)
#    include <iomanip>
#else
#    ifdef _MSC_VER
#        pragma message("<iomanip> not available")
#    else
#        warning "<iomanip> not available"
#    endif
#endif

#if __has_include(<streambuf>)
#    include <streambuf>
#else
#    ifdef _MSC_VER
#        pragma message("<streambuf> not available")
#    else
#        warning "<streambuf> not available"
#    endif
#endif

#if __has_include(<cstdio>)
#    include <cstdio>
#else
#    ifdef _MSC_VER
#        pragma message("<cstdio> not available")
#    else
#        warning "<cstdio> not available"
#    endif
#endif

//Filesystem
#if __has_include(<filesystem>)
#    include <filesystem> //C++17
#else
#    ifdef _MSC_VER
#        pragma message("<filesystem> not available (C++17)")
#    else
#        warning "<filesystem> not available (C++17)"
#    endif
#endif

//Regexp
#if __has_include(<regex>)
#    include <regex> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<regex> not available (C++11)")
#    else
#        warning "<regex> not available (C++11)"
#    endif
#endif

//Atomic
#if __has_include(<atomic>)
#    include <atomic> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<atomic> not available (C++11)")
#    else
#        warning "<atomic> not available (C++11)"
#    endif
#endif

//Thread
#if __has_include(<thread>)
#    include <thread> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<thread> not available (C++11)")
#    else
#        warning "<thread> not available (C++11)"
#    endif
#endif

#if __has_include(<stop_token>)
#    include <stop_token> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<stop_token> not available (C++20)")
#    else
#        warning "<stop_token> not available (C++20)"
#    endif
#endif

#if __has_include(<mutex>)
#    include <mutex> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<mutex> not available (C++11)")
#    else
#        warning "<mutex> not available (C++11)"
#    endif
#endif

#if __has_include(<shared_mutex>)
#    include <shared_mutex> //C++14
#else
#    ifdef _MSC_VER
#        pragma message("<shared_mutex> not available (C++14)")
#    else
#        warning "<shared_mutex> not available (C++14)"
#    endif
#endif

#if __has_include(<future>)
#    include <future> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<future> not available (C++11)")
#    else
#        warning "<future> not available (C++11)"
#    endif
#endif

#if __has_include(<condition_variable>)
#    include <condition_variable> //C++11
#else
#    ifdef _MSC_VER
#        pragma message("<condition_variable> not available (C++11)")
#    else
#        warning "<condition_variable> not available (C++11)"
#    endif
#endif

#if __has_include(<semaphore>)
#    include <semaphore> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<semaphore> not available (C++20)")
#    else
#        warning "<semaphore> not available (C++20)"
#    endif
#endif

#if __has_include(<latch>)
#    include <latch> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<latch> not available (C++20)")
#    else
#        warning "<latch> not available (C++20)"
#    endif
#endif

#if __has_include(<barrier>)
#    include <barrier> //C++20
#else
#    ifdef _MSC_VER
#        pragma message("<barrier> not available (C++20)")
#    else
#        warning "<barrier> not available (C++20)"
#    endif
#endif


#endif
