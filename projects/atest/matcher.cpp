#ifndef __clang__
export module atest : matcher;
export import : matcher_base;
#endif

namespace atest
{
//! The Matcher class is the implementation of an
//! atest matcher. It uses `operator==` to match
//! the values. It also provides extended diff
//! reporting for string convertible values.
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
        if constexpr (Matcher::is_string_convertible<ActualValueT, ExpectedValueT>())
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
        if constexpr (Matcher::is_string_convertible<ActualValueT, ExpectedValueT>())
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
        if constexpr (Matcher::is_string_convertible<ActualValueT, ExpectedValueT>())
        {
            return Matcher::printable_hint(actualValue, expectedValue);
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

        constexpr std::size_t base = 10;
        std::size_t digits = 0;

        while (number != 0)
        {
            number /= base;
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

    [[nodiscard]] static auto diff_pivot(std::size_t diffPos) -> std::size_t
    {
        if (diffPos > Matcher::MAX_STRING_SIZE)
        {
            const std::size_t start = Matcher::diff_start(diffPos);
            return Matcher::left_ellipsis(start).size() + Matcher::HALF_MAX_STRING_SIZE;
        }

        return diffPos;
    }

    [[nodiscard]] static auto diff_start(std::size_t diffPos) -> std::size_t
    {
        if (diffPos < Matcher::HALF_MAX_STRING_SIZE)
        {
            return 0;
        }

        return diffPos - Matcher::HALF_MAX_STRING_SIZE;
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

    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] constexpr static auto is_string_convertible() -> bool
    {
        return ::atest::StringConvertible<ActualValueT> && ::atest::StringConvertible<ExpectedValueT> && !(std::is_pointer_v<ActualValueT> && std::is_pointer_v<ExpectedValueT>);
    }

    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto printable_hint(const ActualValueT &actualValue, const ExpectedValueT &expectedValue) -> std::string
    {
        const std::string actualString = ::atest::stringify(actualValue);
        const std::string expectedString = ::atest::stringify(expectedValue);
        const std::size_t diffPos = Matcher::diff_pos(actualString, expectedString);
        const std::size_t padding = Matcher::printable_string_size(actualString).size() + Matcher::left_padding(actualString.size(), expectedString.size());
        const std::size_t pivot = Matcher::diff_pivot(diffPos);
        return std::string(padding + pivot, ' ') + '^' + std::to_string(diffPos);
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
        if (value.size() < Matcher::MAX_STRING_SIZE)
        {
            return value;
        }

        return Matcher::printable_string_value_long(value, diffPos);
    }

    [[nodiscard]] static auto printable_string_value_long(const std::string &value, std::size_t diffPos) -> std::string
    {
        const std::size_t start = Matcher::diff_start(diffPos);
        return Matcher::left_ellipsis(start)
            + value.substr(start, Matcher::MAX_STRING_SIZE)
            + Matcher::right_ellipsis(start, value.size());
    }

    template<typename ValueT, typename CompareValueT>
    [[nodiscard]] static auto printable_value(const ValueT &value, const CompareValueT &comparedValue) -> std::string
    {
        const std::string valueString = ::atest::stringify(value);
        const std::string compareString = ::atest::stringify(comparedValue);
        const std::size_t diffPos = Matcher::diff_pos(valueString, compareString);
        const std::size_t padding = Matcher::left_padding(valueString.size(), compareString.size());
        return Matcher::printable_string(valueString, diffPos, padding);
    }

    [[nodiscard]] static auto right_ellipsis(std::size_t start, std::size_t end) -> std::string
    {
        if ((start + Matcher::MAX_STRING_SIZE) < end)
        {
            return " [...]";
        }

        return {};
    }

    constexpr static std::size_t MAX_STRING_SIZE = 80;
    constexpr static std::size_t HALF_MAX_STRING_SIZE = (Matcher::MAX_STRING_SIZE / 2);
};
}
