#ifdef _MSC_VER
export module abuild : warning;
export import<astl.hpp>;
#endif

namespace abuild
{
export struct Warning
{
    std::string component;
    std::string what;
};
}
