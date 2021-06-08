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
    std::vector<std::string> compilerFlags;
    std::filesystem::path linker;
    std::vector<std::string> linkerFlags;
    std::filesystem::path archiver;
    std::vector<std::string> archiverFlags;
    std::filesystem::path includePath;
};
}
