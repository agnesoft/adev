#ifndef __clang__
export module atest : stringify;
export import astl;
#endif

namespace atest
{
//! \private
template<typename T>
concept Stringifiable = requires(const T &type)
{
    {std::ostringstream{} << type};
};

//! \private
template<typename T>
concept Iterable = requires(const T &type)
{
    {type.begin()};
    {type.end()};
};

//! Streaming operator for `std::pair`.
export template<typename T1, typename T2>
auto operator<<(std::ostream &stream, const std::pair<T1, T2> &value) -> std::ostream &
{
    return stream << '{' << value.first << ", " << value.second << '}';
}

//! Streaming operator for `std::ostream` that
//! requires container `T` to satisfy the `Iterable`
//! concept. It streams the values of the
//! container wrapped in curly braces and
//! separated by comma. Returns the reference to
//! the passed in `stream`.
//!
//! Examples: `std::vector<int>{1, 2, 3}` will
//! become "{1, 2, 3}"
//!
export template<typename T>
requires(!Stringifiable<T> && Iterable<T>) auto operator<<(std::ostream &stream, const T &container) -> std::ostream &
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
template<typename T, typename... Values>
auto stringify_impl(std::stringstream &stream, const T &value, const Values &...values) -> void
{
    stream << value; //NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

    if constexpr (sizeof...(Values) > 0)
    {
        ::atest::stringify_impl(stream, values...);
    }
}

//! Concatenates `values` by streaming them into
//! `std::ostringstream`. Each value `T` must be
//! streamable. There is a streaming operator
//! provided for containers. Returns the resulting
//! string.
//!
//! Example: `atest::stringify('I', ' ', "am ",
//! 99); //"I am 99"`
template<typename... T>
[[nodiscard]] auto stringify(const T &...values) -> std::string
{
    std::stringstream stream;
    ::atest::stringify_impl(stream, values...);
    return stream.str();
}
}
