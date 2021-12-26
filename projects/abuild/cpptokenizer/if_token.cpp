#ifndef __clang__
export module abuild.cpptokenizer : if_token;
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
    //! Define name.
    std::string name;

    //! Expected define value.
    std::string value;
};

//! Left bracket `(`.
export struct LeftBracketToken
{
};

//! Conditional token checking that a particular
//! define is set to a value greater than or equal
//! to a threshold. The threshold can be a name of
//! a define.
export struct GreaterThanOrEqualsToken
{
    //! Define name.
    std::string name;

    //! Threshold value.
    std::string value;
};

//! Conditional token checking that a particular
//! define is set to a value greater than a
//! threshold. The threshold can be a name of a
//! define.
export struct GreaterThanToken
{
    //! Define name.
    std::string name;

    //! Threshold value.
    std::string value;
};

//! Conditional token checking that a particular
//! define is set to a value lower than or equal
//! to a threshold. The threshold can be a name of
//! a define.
export struct LessThanOrEqualsToken
{
    //! Define name.
    std::string name;

    //! Threshold value.
    std::string value;
};

//! Conditional token checking that a particular
//! define is set to a value lower than a
//! threshold. The threshold can be a name of a
//! define.
export struct LessThanToken
{
    //! Define name.
    std::string name;

    //! Threshold value.
    std::string value;
};

//! Conditional token representing negation of the
//! following token (e.g. `#ifndef MY_MACRO` or
//! `#if !defined(MY_MACRO)`).
export struct NotToken
{
};

//! Conditional token checking that a particular
//! define is not set to certain value.
export struct NotEqualsToken
{
    //! Define name.
    std::string name;

    //! Not expected define value.
    std::string value;
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
    NotToken,
    DefinedToken,
    AndToken,
    OrToken,
    LeftBracketToken,
    RightBracketToken,
    EqualsToken,
    NotEqualsToken,
    GreaterThanToken,
    GreaterThanOrEqualsToken,
    LessThanToken,
    LessThanOrEqualsToken>;

//! IfToken represents a condition as an
//! ordered list of IfElements.
export struct IfToken
{
    //! Condition elements.
    std::vector<IfElement> elements;
};

//! EndIfToken represents an end of a condition.
export struct EndIfToken
{
};
}
