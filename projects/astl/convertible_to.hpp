#ifndef ASTL_CONVERTIBLE_TO_HPP
#define ASTL_CONVERTIBLE_TO_HPP

#include <type_traits>

namespace std //NOLINT(cert-dcl58-cpp)
{
export template<class From, class To>
concept convertible_to = is_convertible_v<From, To> && requires(add_rvalue_reference_t<From>(&f)())
{
    static_cast<To>(f());
};
}

#endif
