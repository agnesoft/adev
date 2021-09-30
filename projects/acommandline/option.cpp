#ifndef __clang__
export module acommandline : option;
export import astl;
#endif

namespace acommandline
{
using DefaultValue = std::variant<std::monostate,
                                  bool,
                                  std::int64_t,
                                  double,
                                  std::string,
                                  std::vector<std::int64_t>,
                                  std::vector<double>,
                                  std::vector<std::string>>;

using DefaultValueArg = std::variant<bool,
                                     std::int64_t,
                                     double,
                                     std::string,
                                     std::vector<std::int64_t>,
                                     std::vector<double>,
                                     std::vector<std::string>>;

using BoundValue = std::variant<std::monostate,
                                bool *,
                                std::int64_t *,
                                double *,
                                std::string *,
                                std::vector<std::int64_t> *,
                                std::vector<double> *,
                                std::vector<std::string> *>;

using BoundValueArg = std::variant<bool *,
                                   std::int64_t *,
                                   double *,
                                   std::string *,
                                   std::vector<std::int64_t> *,
                                   std::vector<double> *,
                                   std::vector<std::string> *>;

static constexpr const char *const POSITIONAL_LONG_NAME = "[positional]";

//! \private
export struct Option
{
    std::string longName;
    std::string description;
    DefaultValue defaultValue;
    BoundValue boundValue;
    bool required = false;
    bool matched = false;
    char shortName = {};
};

[[nodiscard]] auto is_short_name(unsigned char shortName) -> bool
{
    return std::isalpha(shortName) != 0;
}

[[nodiscard]] auto is_long_name(const std::string &name) -> bool
{
    const auto isNotAlphanumeric = [](char c) {
        return std::isalnum(c) == 0 && c != '_' && c != '-' && c != '.';
    };

    return name.size() > 1
        && ::acommandline::is_short_name(name.front())
        && std::find_if(++name.begin() + 1, name.end(), isNotAlphanumeric) == name.end();
}

[[nodiscard]] constexpr auto is_repeated(const Option &option) noexcept -> bool
{
    return std::holds_alternative<std::vector<std::int64_t> *>(option.boundValue)
        || std::holds_alternative<std::vector<double> *>(option.boundValue)
        || std::holds_alternative<std::vector<std::string> *>(option.boundValue);
}

[[nodiscard]] auto is_positional(const Option &option) noexcept -> bool
{
    return option.longName == POSITIONAL_LONG_NAME;
}

[[nodiscard]] constexpr auto is_defaulted(const Option &option) noexcept -> bool
{
    return !std::holds_alternative<std::monostate>(option.defaultValue);
}

[[nodiscard]] constexpr auto is_switch(const Option &option) noexcept -> bool
{
    return std::holds_alternative<bool *>(option.boundValue);
}
}
