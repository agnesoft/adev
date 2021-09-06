#ifndef ASTL_CONVERTIBLE_TO_HPP
#define ASTL_CONVERTIBLE_TO_HPP

#include <type_traits>

namespace std
{
export template<class _From, class _To>
concept convertible_to = is_convertible_v<_From, _To> && requires(add_rvalue_reference_t<_From>(&__f)())
{
    static_cast<_To>(__f());
};
}

#endif
