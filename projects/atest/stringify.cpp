#ifdef _MSC_VER
export module atest : stringify;

export import "astl.hpp";
#endif

namespace atest
{
export template<typename T>
concept stringifiable = requires(const T &type)
{
    {std::ostringstream{} << type};
};

export template<typename T>
concept iterable = requires(const T &type)
{
    {type.begin()};
    {type.end()};
};

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

export template<typename T, typename... Values>
auto stringifyImpl(std::stringstream &stream, const T &value, const Values &...values) -> void
{
    stream << value; //NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

    if constexpr (sizeof...(Values) > 0)
    {
        stringifyImpl(stream, values...);
    }
}

export template<typename... T>
[[nodiscard]] auto stringify(const T &...values) -> std::string
{
    std::stringstream stream;
    stringifyImpl(stream, values...);
    return stream.str();
}
}
