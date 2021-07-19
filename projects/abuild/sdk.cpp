#ifdef _MSC_VER
export module abuild : sdk;
export import<astl.hpp>;
#endif

namespace abuild
{
export struct SDK
{
    std::string name;
    std::string toolchain;
    std::filesystem::path includePath;
    std::filesystem::path interfacePath;
    std::filesystem::path libPath;
};
}
