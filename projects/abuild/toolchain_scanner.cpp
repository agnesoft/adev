#ifdef _MSC_VER
export module abuild : toolchain_scanner;

import : build_cache;
#endif

namespace abuild
{
export class ToolchainScanner
{
public:
    explicit ToolchainScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
#ifdef _MSC_VER
        detectMSVC();
        detectClangWindows();
#else
        detectGCC();
        detectClangLinux();
#endif
    }

private:
    [[nodiscard]] auto arFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto clangFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("-std=c++20", mBuildCache.allocator()); //C++20
        flags.PushBack("-Wall", mBuildCache.allocator()); //All default warnings
        flags.PushBack("-Wextra", mBuildCache.allocator()); //Extra warnings
        flags.PushBack("-pedantic", mBuildCache.allocator()); //Stick to the C++ standard
        flags.PushBack("-Werror", mBuildCache.allocator()); //Warnings are errors
        flags.PushBack("-fmodules", mBuildCache.allocator()); //Enables C++ modules
        return flags;
    }

    [[nodiscard]] auto clangToolchainLinux(const std::string &version) -> rapidjson::Value
    {
        const std::filesystem::path binPath = "/usr/bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("type", rapidjson::Value{"clang" + version, mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("compiler", rapidjson::Value{(binPath / ("clang++-" + version)).lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("compiler_flags", clangFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", rapidjson::Value{(binPath / "ld").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("linker_flags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", rapidjson::Value{(binPath / "ar").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("archiver_flags", arFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    [[nodiscard]] auto clangToolchainWindows(const std::filesystem::path &path) -> rapidjson::Value
    {
        const std::filesystem::path binPath = path / "bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("type", "clang", mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / "clang++.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compiler_flags", clangFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "lld.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linker_flags", lldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "llvm-ar.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiver_flags", llvmArFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    [[nodiscard]] auto clFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/nologo", mBuildCache.allocator()); //No compiler chit chat
        flags.PushBack("/std:c++latest", mBuildCache.allocator()); //C++20 and beyond
        flags.PushBack("/W4", mBuildCache.allocator()); //Highest warning level
        flags.PushBack("/WX", mBuildCache.allocator()); //Warnings are errors
        flags.PushBack("/EHsc", mBuildCache.allocator()); //Enable C++ exceptions
        return flags;
    }

    auto detectClangLinux() -> void
    {
        detectClangVersion("11");
        detectClangVersion("12");
    }

    auto detectClangVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/clang++-" + version))
        {
            mBuildCache["toolchains"].AddMember(rapidjson::Value{"clang" + version, mBuildCache.allocator()}, clangToolchainLinux(version), mBuildCache.allocator());
        }
    }

    auto detectClangWindows() -> void
    {
        const std::filesystem::path clangRoot = "C:/Program Files/LLVM/";

        if (std::filesystem::exists(clangRoot))
        {
            mBuildCache["toolchains"].AddMember("clang", clangToolchainWindows(clangRoot), mBuildCache.allocator());
        }
    }

    auto detectGCCVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/g++-" + version))
        {
            mBuildCache["toolchains"].AddMember(rapidjson::Value{"gcc" + version, mBuildCache.allocator()}, gccToolchain(version), mBuildCache.allocator());
        }
    }

    auto detectMSVC() -> void
    {
        detectMSVC2019();
    }

    auto detectMSVC2019() -> void
    {
        detectMSVC2019Edition("community");
        detectMSVC2019Edition("professional");
        detectMSVC2019Edition("enterprise");
    }

    auto detectMSVC2019Edition(const std::string &edition) -> void
    {
        const std::filesystem::path msvcRoot = "C:/Program Files (x86)/Microsoft Visual Studio/2019/" + edition + "/VC/Tools/MSVC/";

        if (std::filesystem::exists(msvcRoot))
        {
            for (const std::filesystem::directory_entry &toolset : std::filesystem::directory_iterator(msvcRoot))
            {
                mBuildCache["toolchains"].AddMember(rapidjson::Value{"msvc19-" + edition + '-' + toolset.path().filename().string(), mBuildCache.allocator()}, msvcToolchain(toolset.path()), mBuildCache.allocator());
            }
        }
    }

    [[nodiscard]] auto gccFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("-std=c++20", mBuildCache.allocator()); //C++20
        flags.PushBack("-Wall", mBuildCache.allocator()); //All default warnings
        flags.PushBack("-Wextra", mBuildCache.allocator()); //Extra warnings
        flags.PushBack("-pedantic", mBuildCache.allocator()); //Stick to the C++ standard
        flags.PushBack("-Werror", mBuildCache.allocator()); //Warnings are errors
        return flags;
    }

    [[nodiscard]] auto gccToolchain(const std::string &version) -> rapidjson::Value
    {
        const std::filesystem::path binPath = "/usr/bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("type", "gcc", mBuildCache.allocator());
        toolchain.AddMember("compiler", rapidjson::Value{(binPath / ("g++-" + version)).lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("compiler_flags", gccFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", rapidjson::Value{(binPath / "ld").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("linker_flags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", rapidjson::Value{(binPath / "ar").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("archiver_flags", arFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "/usr/include/c++/" + version, mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    [[nodiscard]] auto ldFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto libFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/NOLOGO", mBuildCache.allocator()); //No archiver chitchat
        return flags;
    }

    [[nodiscard]] auto linkFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/WX", mBuildCache.allocator()); //Warnings are errors
        return flags;
    }

    [[nodiscard]] auto lldFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto llvmArFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto msvcToolchain(const std::filesystem::path &path) -> rapidjson::Value
    {
        const std::filesystem::path binPath = path / "bin" / "HostX64" / "x64";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("type", "msvc", mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / "cl.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compiler_flags", clFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "link.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linker_flags", linkFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "lib.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiver_flags", libFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", (path / "ifc").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("include", (path / "include").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("lib", (path / "lib" / "x64").lexically_normal().string(), mBuildCache.allocator());
        return toolchain;
    }

    BuildCache &mBuildCache;
};
}
