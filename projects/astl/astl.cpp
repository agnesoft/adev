export module astl;

export import "astl.hpp";

#if defined(__clang__) || !__has_include(<source_location>)
#    include "source_location.hpp"
#endif
