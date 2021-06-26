#ifdef _MSC_VER
export module abuild : toolchain;
export import<astl.hpp>;
#endif

namespace abuild
{
export struct Toolchain
{
    enum class Type
    {
        Clang,
        GCC,
        MSVC
    };

    std::string name;
    Type type = Type::Clang;
    std::filesystem::path compiler;
    std::filesystem::path linker;
    std::filesystem::path archiver;
    std::unordered_set<std::string> compilerFlags;
    std::unordered_set<std::string> linkerFlags;
    std::unordered_set<std::string> archiverFlags;
    std::filesystem::path includePath;
    std::filesystem::path libPath;
};
}
