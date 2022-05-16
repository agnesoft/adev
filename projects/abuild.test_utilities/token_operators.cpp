#ifndef __clang__
export module abuild.test_utilities:token_operators;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const AndToken &left, [[maybe_unused]] const AndToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const DefinedToken &left, const DefinedToken &right) noexcept -> bool
{
    return left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const EqualsToken &left, [[maybe_unused]] const EqualsToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const GreaterThanOrEqualsToken &left, const GreaterThanOrEqualsToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const GreaterThanToken &left, const GreaterThanToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const LeftBracketToken &left, [[maybe_unused]] const LeftBracketToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const LessThanOrEqualsToken &left, const LessThanOrEqualsToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const LessThanToken &left, const LessThanToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const HasIncludeLocalToken &left, const HasIncludeLocalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const HasIncludeExternalToken &left, const HasIncludeExternalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const NotToken &left, [[maybe_unused]] const NotToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const OrToken &left, [[maybe_unused]] const OrToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const RightBracketToken &left, [[maybe_unused]] const RightBracketToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const IfToken &left, const IfToken &right) noexcept -> bool
{
    return left.elements == right.elements;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const ElseToken &left, [[maybe_unused]] const ElseToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==([[maybe_unused]] const EndIfToken &left, [[maybe_unused]] const EndIfToken &right) noexcept -> bool
{
    return true;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const DefineToken &left, const DefineToken &right) noexcept -> bool
{
    return left.name == right.name && left.value == right.value;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ImportIncludeExternalToken &left, const ImportIncludeExternalToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ImportIncludeLocalToken &left, const ImportIncludeLocalToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ImportModulePartitionToken &left, const ImportModulePartitionToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ImportModuleToken &left, const ImportModuleToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const IncludeExternalToken &left, const IncludeExternalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const IncludeLocalToken &left, const IncludeLocalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ModulePartitionToken &left, const ModulePartitionToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.mod == right.mod && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const ModuleToken &left, const ModuleToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

//! \private
export [[nodiscard]] constexpr auto operator==(const UndefToken &left, const UndefToken &right) noexcept -> bool
{
    return left.name == right.name;
}
}
