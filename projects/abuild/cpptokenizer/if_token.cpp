#ifndef __clang__
export module abuild.cpptokenizer : if_token;
export import astl;
#endif

namespace abuild
{
//! Logical operator `&&`.
export struct AndToken
{
    friend auto operator==(const AndToken &, const AndToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set. E.g. `#ifdef MY_MACRO` or `#if
//! defined(MY_MACRO)`.
export struct DefinedToken
{
    //! Define name.
    std::string name;

    friend auto operator==(const DefinedToken &, const DefinedToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set to certain value. The value `1`
//! is the default for defined values without a
//! specific value. E.g. `#ifdef MY_MACRO` is
//! equivalent to `#if MY_MACRO == 1`.
export struct EqualsToken
{
    //! Left value.
    std::string left;

    //! Right value.
    std::string right;

    friend auto operator==(const EqualsToken &, const EqualsToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set to a value greater than or equal
//! to a threshold. The threshold can be a name of
//! a define.
export struct GreaterThanOrEqualsToken
{
    //! Left value.
    std::string left;

    //! Right value.
    std::string right;

    friend auto operator==(const GreaterThanOrEqualsToken &, const GreaterThanOrEqualsToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set to a value greater than a
//! threshold. The threshold can be a name of a
//! define.
export struct GreaterThanToken
{
    //! Left value.
    std::string left;

    //! Right value.
    std::string right;

    friend auto operator==(const GreaterThanToken &, const GreaterThanToken &) -> bool = default;
};

//! Left bracket `(`.
export struct LeftBracketToken
{
    friend auto operator==(const LeftBracketToken &, const LeftBracketToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set to a value lower than or equal
//! to a threshold. The threshold can be a name of
//! a define.
export struct LessThanOrEqualsToken
{
    //! Left value.
    std::string left;

    //! Right value.
    std::string right;

    friend auto operator==(const LessThanOrEqualsToken &, const LessThanOrEqualsToken &) -> bool = default;
};

//! Conditional token checking that a particular
//! define is set to a value lower than a
//! threshold. The threshold can be a name of a
//! define.
export struct LessThanToken
{
    //! Left value.
    std::string left;

    //! Right value.
    std::string right;

    friend auto operator==(const LessThanToken &, const LessThanToken &) -> bool = default;
};

//! Conditional token checking that a `include`
//! path is available locally.
export struct HasIncludeLocalToken
{
    //! Include file path.
    std::string include;

    friend auto operator==(const HasIncludeLocalToken &, const HasIncludeLocalToken &) -> bool = default;
};

//! Conditional token checking that a `include`
//! path is available externally.
export struct HasIncludeExternalToken
{
    //! Include file path.
    std::string include;

    friend auto operator==(const HasIncludeExternalToken &, const HasIncludeExternalToken &) -> bool = default;
};

//! Conditional token representing negation of the
//! following token (e.g. `#ifndef MY_MACRO` or
//! `#if !defined(MY_MACRO)`).
export struct NotToken
{
    friend auto operator==(const NotToken &, const NotToken &) -> bool = default;
};

//! Logical operator `||`.
export struct OrToken
{
    friend auto operator==(const OrToken &, const OrToken &) -> bool = default;
};

//! Right bracket `)`.
export struct RightBracketToken
{
    friend auto operator==(const RightBracketToken &, const RightBracketToken &) -> bool = default;
};

//! IfElement is a variant type combining all
//! possible condition token types.
export using IfElement = std::variant<
    AndToken,
    DefinedToken,
    EqualsToken,
    GreaterThanOrEqualsToken,
    GreaterThanToken,
    HasIncludeExternalToken,
    HasIncludeLocalToken,
    LeftBracketToken,
    LessThanOrEqualsToken,
    LessThanToken,
    OrToken,
    NotToken,
    RightBracketToken>;

//! IfToken represents a condition as an
//! ordered list of IfElements.
export struct IfToken
{
    //! Condition elements.
    std::vector<IfElement> elements;

    friend auto operator==(const IfToken &, const IfToken &) -> bool = default;
};

//! ElseToken represents an preprocessor `#else`
//! including `#elif`, `#elifdef` and `#elifndef`.
export struct ElseToken
{
    friend auto operator==(const ElseToken &, const ElseToken &) -> bool = default;
};

//! EndIfToken represents an end of a condition.
export struct EndIfToken
{
    friend auto operator==(const EndIfToken &, const EndIfToken &) -> bool = default;
};
}
