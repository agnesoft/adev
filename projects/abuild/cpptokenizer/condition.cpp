#ifndef __clang__
export module abuild.cpptokenizer : condition;
export import astl;
#endif

namespace abuild
{
export struct AndToken
{
};

export struct OrToken
{
};

export struct LeftBracketToken
{
};

export struct RightBracketToken
{
};

export struct NotDefinedToken
{
    std::string name;
};

export struct EqualsToken
{
    std::string name;
    std::string value;
};

export struct NotEqualsToken
{
    std::string name;
    std::string value;
};

export struct GreaterThanToken
{
    std::string name;
    std::int64_t value = 0;
};

export struct GreaterThanOrEqualsToken
{
    std::string name;
    std::int64_t value = 0;
};

export struct LessThanToken
{
    std::string name;
    std::int64_t value = 0;
};

export struct LessThanOrEqualsToken
{
    std::string name;
    std::int64_t value = 0;
};

using ConditionElement = std::variant<
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

export struct ConditionToken
{
    std::vector<ConditionElement> elements;
};
}
