#ifdef _MSC_VER
export module atest : stringify;

export import "astl.hpp";
#endif

namespace atest
{
//! Concept that is `true` if `T` can be
//! streamed into `std::ostringstream`.
export template<typename T>
concept stringifiable = requires(const T &type)
{
    {std::ostringstream{} << type};
};

//! Concept that is `true` if `T` has
//! `begin()` and `end()` methods.
export template<typename T>
concept iterable = requires(const T &type)
{
    {type.begin()};
    {type.end()};
};

//! Streaming operator for `std::ostream` that
//! requires container `T` to satisfy the iterable
//! concept. It streams the values of the container
//! wrapped in curly braces and separated by comma.
//! Returns the reference to the passed in `stream`.
//!
//! Examples: `std::vector<int>{1, 2, 3}` will become "{1, 2, 3}"
//! 
export template<typename T>
requires(!stringifiable<T> && iterable<T>) auto operator<<(std::ostream &stream, const T &container) -> std::ostream &
{
    stream << '{';

    for (auto it = container.begin(); it != container.end();)
    {
        stream << (*it);

        if (++it != container.end())
        {
            stream << ", ";
        }
    }

    stream << '}';
    return stream;
}

//! \private
export template<typename T, typename... Values>
auto stringifyImpl(std::stringstream &stream, const T &value, const Values &...values) -> void
{
    stream << value; //NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

    if constexpr (sizeof...(Values) > 0)
    {
        stringifyImpl(stream, values...);
    }
}

//! Concatenates `values` by streaming them into `std::ostringstream`.
//! Each value `T` must be streamable. There is a streaming operator
//! provided for containers. Returns the resulting string.
//!
//! Example: `atest::stringify('I', ' ', "am ", 99); //"I am 99"`
export template<typename... T>
[[nodiscard]] auto stringify(const T &...values) -> std::string
{
    std::ostringstream stream;
    stringifyImpl(stream, values...);
    return stream.str();
}
}
