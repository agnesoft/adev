#ifndef __clang__
export module abuild.cpptokenizer : condition;
export import astl;
#endif

namespace abuild
{
//! Logical operator `&&`.
export struct AndToken
{
};

//! Logical operator `||`.
export struct OrToken
{
};

//! Left bracket `(`.
export struct LeftBracketToken
{
};

//! Right bracket `)`.
export struct RightBracketToken
{
};

//! Conditional token checking that a particular
//! define is not set. E.g. `#ifndef MY_MACRO` or
//! `#if !defined(MY_MACRO)`
export struct NotDefinedToken
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

//! Conditional token checking that a particular
//! define is not set to certain value.
export struct NotEqualsToken
{
    //! Define name.
    std::string name;

    //! Not expected define value.
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

//! ConditionElement is a variant type combining
//! all possible condition token types.
export using ConditionElement = std::variant<
    AndToken,
    OrToken,
    LeftBracketToken,
    RightBracketToken,
    NotDefinedToken,
    EqualsToken,
    NotEqualsToken,
    GreaterThanToken,
    GreaterThanOrEqualsToken,
    LessThanToken,
    LessThanOrEqualsToken>;

//! ConditionToken represents a condition as an
//! ordered list of ConditionElements.
export struct ConditionToken
{
    //! Condition elements.
    std::vector<ConditionElement> elements;
};
}
