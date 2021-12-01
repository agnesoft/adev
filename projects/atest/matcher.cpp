#ifndef __clang__
export module atest : matcher;
export import : matcher_base;
#endif

namespace atest
{
//! The Matcher class is the implementation of an
//! atest matcher. It uses `operator==` to match
//! the values.
export class Matcher : public MatcherBase
{
public:
    //! Returns the actual value received. If the
    //! value is string convertible it will
    //! perform a diff on the values and print the
    //! `actual` diff with a difference hint
    //! otherwise it falls back on the
    //! MatcherBase.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto actual(const ActualValueT &actualValue, const ExpectedValueT &expectedValue) -> std::string
    {
        if constexpr (::atest::StringConvertible<ActualValueT> && ::atest::StringConvertible<ExpectedValueT>)
        {
            return Matcher::printable_string(actualValue, expectedValue);
        }
        else
        {
            return MatcherBase::actual(actualValue, expectedValue);
        }
    }

    //! Returns the expected value. If the value
    //! is string convertible it will perform a
    //! diff on the values and print the
    //! `expected` diff otherwise it falls back on
    //! the MatcherBase.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto expected(const ActualValueT &actualValue, const ExpectedValueT &expectedValue) -> std::string
    {
        if constexpr (::atest::StringConvertible<ActualValueT> && ::atest::StringConvertible<ExpectedValueT>)
        {
            return Matcher::printable_string(expectedValue, actualValue);
        }
        else
        {
            return MatcherBase::expected(actualValue, expectedValue);
        }
    }

    //! If the values are strings or string
    //! convertible types returns the caret
    //! pointing to the position of the first
    //! difference.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto hint(const ActualValueT &actualValue, const ExpectedValueT &expectedValue) -> std::string
    {
        if constexpr (::atest::StringConvertible<ActualValueT> && ::atest::StringConvertible<ExpectedValueT>)
        {
            const std::string actualString{actualValue};
            const std::string expectedString{expectedValue};
            const std::size_t diff = Matcher::diff_pos(actualString, expectedString);
            const std::size_t pivot = Matcher::printable_string(actualString, diff).second;

            std::stringstream out;
            out << std::setw(pivot) << '^' << diff;
            return out.str();
        }
        else
        {
            return MatcherBase::hint(actualValue, expectedValue);
        }
    }

    //! Returns `true` if the `actual == expected`.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] auto operator()(const ActualValueT &actual, const ExpectedValueT &expected) const -> bool
    {
        return actual == expected;
    }

private:
    [[nodiscard]] static auto diff_pos(const std::string &left, const std::string &right) -> std::size_t
    {
        std::size_t limit = std::min(left.size(), right.size());

        for (std::size_t i = 0; i < limit; ++i)
        {
            if (left[i] != right[i])
            {
                return i;
            }
        }

        return limit;
    }

    template<typename ValueT, typename ComapareValueT>
    [[nodiscard]] static auto printable_string(const ValueT &value, const ComapareValueT &comparedValue) -> std::string
    {
        const std::string valueString{value};
        const std::string compareString{comparedValue};
        const std::size_t diff = Matcher::diff_pos(valueString, compareString);
        return Matcher::printable_string(valueString, diff).first;
    }

    [[nodiscard]] static auto printable_string(const std::string &value, std::size_t diff) -> std::pair<std::string, std::size_t>
    {
        constexpr std::size_t maxSize = 80;

        std::stringstream out;
        out << '(' << value.size() << ") ";

        std::size_t pivot = out.view().size() + 1;

        if (value.size() > maxSize)
        {
            pivot += (maxSize / 2);
            std::size_t start = (diff < (maxSize / 2)) ? 0 : diff - (maxSize / 2);

            if (start != 0)
            {
                out << "[...] ";
                pivot += 6;
            }

            out << value.substr(start, maxSize);

            if ((start + maxSize) < value.size())
            {
                out << " [...]";
            }
        }
        else
        {
            pivot += diff;
            out << value;
        }

        return std::pair<std::string, std::size_t>{out.str(), pivot};
    }
};
}
