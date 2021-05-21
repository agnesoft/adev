#ifdef _MSC_VER
export module abuild : error;
export import<astl.hpp>;
#endif

namespace abuild
{
export struct Error
{
    std::string component;
    std::string what;
};
}
