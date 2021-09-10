#ifndef __clang__
module acommandline : option_data;
import astl;
#endif

namespace acommandline
{
//! A variant holding a default value to be
//! bound if an option is not matched.
using DefaultValue = std::variant<std::monostate,
                                  bool,
                                  std::int64_t,
                                  double,
                                  std::string,
                                  std::vector<std::int64_t>,
                                  std::vector<double>,
                                  std::vector<std::string>>;

//! A variant holding a pointer to the bound
//! value.
using BoundValue = std::variant<std::monostate,
                                bool *,
                                std::int64_t *,
                                double *,
                                std::string *,
                                std::vector<std::int64_t> *,
                                std::vector<double> *,
                                std::vector<std::string> *>;

struct OptionData
{
    std::string longName;
    std::string description;
    DefaultValue defaultValue;
    BoundValue boundValue;
    bool required = false;
    char shortName = {};
};
}
