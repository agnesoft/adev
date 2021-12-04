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
            return Matcher::printable_value(actualValue, expectedValue);
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
            return Matcher::printable_value(expectedValue, actualValue);
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
            const std::size_t diffPos = Matcher::diff_pos(actualString, expectedString);
            const std::size_t padding = Matcher::left_padding(actualString.size(), expectedString.size());
            const std::size_t pivot = Matcher::diff_pivot(actualString, diffPos, padding);

            std::stringstream out;
            out << std::setw(pivot) << '^' << diffPos;
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
    [[nodiscard]] static auto digit_count(std::size_t number) noexcept -> std::size_t
    {
        if (number == 0)
        {
            return 1;
        }

        std::size_t digits = 0;

        while (number != 0)
        {
            number /= 10;
            ++digits;
        }

        return digits;
    }

    [[nodiscard]] static auto diff_pos(const std::string &left, const std::string &right) -> std::size_t
    {
        const std::size_t limit = std::min(left.size(), right.size());

        for (std::size_t i = 0; i < limit; ++i)
        {
            if (left[i] != right[i])
            {
                return i;
            }
        }

        return limit;
    }

    [[nodiscard]] static auto diff_pivot(const std::string &actualString, std::size_t diffPos, std::size_t padding) -> std::size_t
    {
        std::size_t pivot = Matcher::printable_string_size(actualString).size() + padding + 1;

        if (diffPos > Matcher::maxStringSize)
        {
            const std::size_t start = Matcher::diff_start(diffPos);
            return pivot + Matcher::left_ellipsis(start).size() + Matcher::halfMaxStringSize;
        }

        return pivot + diffPos;
    }

    [[nodiscard]] static auto diff_start(std::size_t diffPos) -> std::size_t
    {
        if (diffPos < Matcher::halfMaxStringSize)
        {
            return 0;
        }

        return diffPos - Matcher::halfMaxStringSize;
    }

    [[nodiscard]] static auto left_ellipsis(std::size_t start) -> std::string
    {
        if (start != 0)
        {
            return "[...] ";
        }

        return {};
    }

    [[nodiscard]] static auto left_padding(std::size_t valueSize, std::size_t otherSize) -> std::size_t
    {
        if (valueSize == otherSize)
        {
            return 0;
        }

        return Matcher::left_padding_unequal(valueSize, otherSize);
    }

    [[nodiscard]] static auto left_padding_unequal(std::size_t valueSize, std::size_t otherSize) -> std::size_t
    {
        const std::size_t stringValueSize = Matcher::digit_count(valueSize);
        const std::size_t stringOtherSize = Matcher::digit_count(otherSize);

        if (stringValueSize < stringOtherSize)
        {
            return stringOtherSize - stringValueSize;
        }

        return 0;
    }

    [[nodiscard]] static auto printable_string(const std::string &value, std::size_t diffPos, std::size_t padding) -> std::string
    {
        return Matcher::printable_string_size(value) + std::string(padding, ' ') + Matcher::printable_string_value(value, diffPos);
    }

    [[nodiscard]] static auto printable_string_size(const std::string &value) -> std::string
    {
        return '(' + std::to_string(value.size()) + ") ";
    }

    [[nodiscard]] static auto printable_string_value(const std::string &value, std::size_t diffPos) -> std::string
    {
        if (value.size() < Matcher::maxStringSize)
        {
            return value;
        }
        else
        {
            return Matcher::printable_string_value_long(value, diffPos);
        }
    }

    [[nodiscard]] static auto printable_string_value_long(const std::string &value, std::size_t diffPos) -> std::string
    {
        const std::size_t start = Matcher::diff_start(diffPos);
        return Matcher::left_ellipsis(start)
            + value.substr(start, Matcher::maxStringSize)
            + Matcher::right_ellipsis(start, value.size());
    }

    template<typename ValueT, typename CompareValueT>
    [[nodiscard]] static auto printable_value(const ValueT &value, const CompareValueT &comparedValue) -> std::string
    {
        const std::string valueString{value};
        const std::string compareString{comparedValue};
        const std::size_t diffPos = Matcher::diff_pos(valueString, compareString);
        const std::size_t padding = Matcher::left_padding(valueString.size(), compareString.size());
        return Matcher::printable_string(valueString, diffPos, padding);
    }

    [[nodiscard]] static auto right_ellipsis(std::size_t start, std::size_t end) -> std::string
    {
        if ((start + Matcher::maxStringSize) < end)
        {
            return " [...]";
        }
        else
        {
            return {};
        }
    }

    constexpr static std::size_t maxStringSize = 80;
    constexpr static std::size_t halfMaxStringSize = (Matcher::maxStringSize / 2);
};
}
