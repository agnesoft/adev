#ifndef __clang__
module acommandline : option_data;
import astl;
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
