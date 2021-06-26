#ifdef _MSC_VER
export module abuild : toolchain_scanner;
export import : build_cache;
#endif

namespace abuild
{
export class ToolchainScanner
{
public:
    explicit ToolchainScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        detectMSVC();
        detectClang();
    }

private:
    auto addToolchain(Toolchain toolchain) -> void
    {
        if (std::filesystem::exists(toolchain.compiler))
        {
            mBuildCache.addToolchain(std::move(toolchain));
        }
    }

    [[nodiscard]] auto clangArchiver(const std::filesystem::path &path, const std::string &suffix) -> std::filesystem::path
    {
        const std::filesystem::path llvmar = path / ("llvm-ar" + suffix);
        return std::filesystem::exists(llvmar) ? llvmar : (path / "ar");
    }

    [[nodiscard]] static auto clangLinker(const std::filesystem::path &path, const std::string &suffix) -> std::filesystem::path
    {
        const std::filesystem::path lld = path / ("lld" + suffix);
        return std::filesystem::exists(lld) ? lld : (path / "ld");
    }

    [[nodiscard]] static auto clangSuffix([[maybe_unused]] const std::string &version) -> std::string
    {
        std::string suffix = (version.empty() ? "" : "-") + version;

#ifdef _WIN32
        return suffix + ".exe";
#else
        return suffix;
#endif
    }

    [[nodiscard]] auto clangIncludeDir(const std::filesystem::path &path, const std::string &version) -> std::filesystem::path
    {
        if (version.empty())
        {
            return clangIncludeDirAutoVersion(path);
        }
        else
        {
            return path / "lib" / "clang" / version / "include";
        }
    }

    [[nodiscard]] auto clangLibDir(const std::filesystem::path &path, const std::string &version) -> std::filesystem::path
    {
        if (version.empty())
        {
            return clangLibDirAutoVersion(path);
        }
        else
        {
            return clangLibDirVersion(path, version);
        }
    }

    [[nodiscard]] auto clangIncludeDirAutoVersion(const std::filesystem::path &path) -> std::filesystem::path
    {
        const std::filesystem::path includeRoot = path / "lib" / "clang";

        if (std::filesystem::exists(includeRoot))
        {
            for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(includeRoot))
            {
                return entry.path() / "include";
            }
        }

        return {};
    }

    [[nodiscard]] auto clangLibDirAutoVersion(const std::filesystem::path &path) -> std::filesystem::path
    {
        const std::filesystem::path libRoot = path / "lib" / "clang";

        if (std::filesystem::exists(libRoot))
        {
            for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(libRoot))
            {
                const std::filesystem::path osPath = entry.path() / "lib";

                for (const std::filesystem::directory_entry &os : std::filesystem::directory_iterator(osPath))
                {
                    return os.path();
                }
            }
        }

        return {};
    }

    [[nodiscard]] auto clangLibDirVersion(const std::filesystem::path &path, const std::string &version) -> std::filesystem::path
    {
        const std::filesystem::path libRoot = path / "lib" / "clang" / version / "lib";

        if (std::filesystem::exists(libRoot))
        {
            for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(libRoot))
            {
                return entry.path();
            }
        }

        return {};
    }

    auto detectClang() -> void
    {
        if (std::filesystem::exists(mBuildCache.settings().clangInstallDirectory()))
        {
            detectClangVersion(mBuildCache.settings().clangInstallDirectory(), "");
            detectClangVersion(mBuildCache.settings().clangInstallDirectory(), "12");
            detectClangVersion(mBuildCache.settings().clangInstallDirectory(), "11");
            detectClangVersion(mBuildCache.settings().clangInstallDirectory(), "10");
        }
    }

    auto detectClangVersion(const std::filesystem::path &path, const std::string &version) -> void
    {
        const std::string suffix = clangSuffix(version);

        addToolchain(Toolchain{
            .name = "clang" + version,
            .type = Toolchain::Type::Clang,
            .compiler = path / "bin" / ("clang++" + suffix),
            .linker = clangLinker(path / "bin", suffix),
            .archiver = clangArchiver(path / "bin", suffix),
            .compilerFlags = {"-std=c++20", "-fmodules", "-Wall", "-Wextra", "-pedantic", "-c", "-x c++"},
            .linkerFlags = {},
            .archiverFlags = {},
            .includePath = clangIncludeDir(path, ""),
            .libPath = clangLibDir(path, "")});
    }

    auto detectMSVC() -> void
    {
        if (std::filesystem::exists(mBuildCache.settings().msvcInstallDirectory()))
        {
            detectMSVCYearEdition("2019", "Enterprise", "x64");
            detectMSVCYearEdition("2019", "Professional", "x64");
            detectMSVCYearEdition("2019", "Community", "x64");
            detectMSVCYearEdition("2019", "Enterprise", "x32");
            detectMSVCYearEdition("2019", "Professional", "x32");
            detectMSVCYearEdition("2019", "Community", "x32");
        }
    }

    auto detectMSVC(const std::filesystem::path &path, const std::string &year, const std::string &edition, const std::string &architecture) -> void
    {
        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
        {
            addToolchain(Toolchain{
                .name = "msvc" + year + '_' + architecture + '_' + edition + '_' + entry.path().filename().string(),
                .type = Toolchain::Type::MSVC,
                .compiler = entry.path() / "bin" / "Hostx64" / architecture / "cl.exe",
                .linker = entry.path() / "bin" / "Hostx64" / architecture / "link.exe",
                .archiver = entry.path() / "bin" / "Hostx64" / architecture / "lib.exe",
                .compilerFlags = {"/nologo", "/EHsc", "/std:c++latest", "/W4", "/c", "/TP"},
                .linkerFlags = {"/NOLOGO"},
                .archiverFlags = {"/NOLOGO"},
                .includePath = entry.path() / "include",
                .libPath = entry.path() / "lib" / architecture});
        }
    }

    auto detectMSVCYearEdition(const std::string &year, const std::string &edition, const std::string &architecture) -> void
    {
        const auto path = std::filesystem::path{mBuildCache.settings().msvcInstallDirectory()} / year / edition / "VC" / "Tools" / "MSVC";

        if (std::filesystem::exists(path))
        {
            detectMSVC(path, year, edition, architecture);
        }
    }

    BuildCache &mBuildCache;
};
}
