#ifndef ASTL_INVOCABLE_HPP
#define ASTL_INVOCABLE_HPP

#include <functional>
#include <type_traits>
#include <utility>

namespace std
{
export template<class F, class... Args>
concept invocable = requires(F && f, Args &&...args)
{
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
};

export template<class _Fn, class... _Args>
concept regular_invocable = invocable<_Fn, _Args...>;
}

#endif
