export module astl;

export import "astl.hpp";

#if defined(__clang__) && !__has_include(<source_location>)
#    include "source_location.hpp"
#endif

#if defined(__clang__) && !__has_include(<__concepts/convertible_to.h>)
#    include "invocable.hpp"
#endif

#if defined(__clang__) && !__has_include(<__concepts/invocable.h>)
#    include "convertible_to.hpp"
#endif