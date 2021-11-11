#if __has_include(<Windows.h>)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    define WIN32_LEAN_AND_MEAN
// clang-format off
#    include <Windows.h>
// clang-format on
#    pragma comment(lib, "User32.Lib")
#endif
