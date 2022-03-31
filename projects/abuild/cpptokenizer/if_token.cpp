#ifndef __clang__
export module abuild.cpptokenizer:if_token;
export import astl;
#endif

namespace abuild
{
//! Logical operator `&&`.
export struct AndToken
{
};

//! Conditional token checking that a particular
//! define is set. E.g. `#ifdef MY_MACRO` or `#if
//! defined(MY_MACRO)`.
export struct DefinedToken
{
    //! Define name.
    std::string name;
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
};

//! Left bracket `(`.
export struct LeftBracketToken
{
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
};

//! Conditional token checking that a `name`
//! path is available locally.
export struct HasIncludeLocalToken
{
    //! Include file path.
    std::string name;
};

//! Conditional token checking that a `include`
//! path is available externally.
export struct HasIncludeExternalToken
{
    //! Include file path.
    std::string name;
};

//! Conditional token representing negation of the
//! following token (e.g. `#ifndef MY_MACRO` or
//! `#if !defined(MY_MACRO)`).
export struct NotToken
{
};

//! Logical operator `||`.
export struct OrToken
{
};

//! Right bracket `)`.
export struct RightBracketToken
{
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
};

//! ElseToken represents an preprocessor `#else`
//! including `#elif`, `#elifdef` and `#elifndef`.
export struct ElseToken
{
};

//! EndIfToken represents an end of a condition.
export struct EndIfToken
{
};
}
