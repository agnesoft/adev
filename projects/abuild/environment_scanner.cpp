#ifdef _MSC_VER
export module abuild : environment_scanner;

import : build_cache;

import acore;
#endif

namespace abuild
{
export class EnvironmentScanner
{
public:
    EnvironmentScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        scan();
    }

private:
    [[nodiscard]] auto llvmArFlags() -> rapidjson::Value
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

    [[nodiscard]] auto lldFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    auto detectGCC() -> void
    {
        mBuildCache["toolchains"].AddMember("gcc", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        detectGCCVersion("9");
        detectGCCVersion("10");
        detectGCCVersion("11");
    }

    auto detectGCCVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/g++-" + version))
        {
            mBuildCache["toolchains"]["gcc"].PushBack(gccToolchain(version), mBuildCache.allocator());
        }
    }

    auto detectLinuxClang() -> void
    {
        mBuildCache["toolchains"].AddMember("clang", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        detectLinuxClangVersion("11");
        detectLinuxClangVersion("12");
    }

    auto detectLinuxClangVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/clang++-" + version))
        {
            mBuildCache["toolchains"]["clang"].PushBack(linuxClangToolchain(version), mBuildCache.allocator());
        }
    }

    auto detectMSVC() -> void
    {
        mBuildCache["toolchains"].AddMember("msvc", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        detectMSVC2019();
    }

    auto detectMSVC2019() -> void
    {
        const std::filesystem::path msvc2019root = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/";

        if (std::filesystem::exists(msvc2019root))
        {
            for (const std::filesystem::directory_entry &version : std::filesystem::directory_iterator(msvc2019root))
            {
                mBuildCache["toolchains"]["msvc"].PushBack(msvcToolchain(version.path()), mBuildCache.allocator());
            }
        }
    }

    [[nodiscard]] auto arFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
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

    [[nodiscard]] auto ldFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto linuxClangToolchain(const std::string &version) -> rapidjson::Value
    {
        const std::filesystem::path binPath = "/usr/bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("name", "clang" + version, mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / ("clang++-" + version)).lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compilerFlags", clangFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "ld").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "ar").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", arFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    [[nodiscard]] auto gccToolchain(const std::string &version) -> rapidjson::Value
    {
        const std::filesystem::path binPath = "/usr/bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("name", "gcc" + version, mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / ("g++-" + version)).lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compilerFlags", gccFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "ld").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "ar").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", arFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", (std::filesystem::path{"/usr/include/c++/"} / version).lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    [[nodiscard]] auto libExeFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/NOLOGO", mBuildCache.allocator()); //No archiver chitchat
        return flags;
    }

    [[nodiscard]] auto clExeFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/nologo", mBuildCache.allocator()); //No compiler chit chat
        flags.PushBack("/std:c++latest", mBuildCache.allocator()); //C++20 and beyond
        flags.PushBack("/W4", mBuildCache.allocator()); //Highest warning level
        flags.PushBack("/WX", mBuildCache.allocator()); //Warnings are errors
        flags.PushBack("/EHsc", mBuildCache.allocator()); //Enable C++ exceptions
        return flags;
    }

    [[nodiscard]] auto linkExeFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/WX", mBuildCache.allocator()); //Warnings are errors
        return flags;
    }

    [[nodiscard]] auto msvcToolchain(const std::filesystem::path &path) -> rapidjson::Value
    {
        const std::filesystem::path binPath = path / "bin" / "HostX64" / "x64";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("name", path.filename().string(), mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / "cl.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compilerFlags", clExeFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "link.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", linkExeFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "lib.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", libExeFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", (path / "ifc").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("include", (path / "include").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("lib", (path / "lib" / "x64").lexically_normal().string(), mBuildCache.allocator());
        return toolchain;
    }

    auto detectWindowsClang() -> void
    {
        const std::filesystem::path clangRoot = "C:/Program Files/LLVM/";

        if (std::filesystem::exists(clangRoot))
        {
            mBuildCache["toolchains"].AddMember("clang", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
            mBuildCache["toolchains"]["clang"].PushBack(windowsClangToolchain(clangRoot), mBuildCache.allocator());
        }
    }

    auto scan() -> void
    {
#ifdef _WIN32
        scanWindows();
#else
        scanLinux();
#endif
    }

    auto scanLinux() -> void
    {
        detectGCC();
        detectLinuxClang();
    }

    auto scanWindows() -> void
    {
        detectMSVC();
        detectWindowsClang();
    }

    [[nodiscard]] auto windowsClangToolchain(const std::filesystem::path &path) -> rapidjson::Value
    {
        const std::filesystem::path binPath = path / "bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("name", "clang", mBuildCache.allocator());
        toolchain.AddMember("compiler", (binPath / "clang++.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("compilerFlags", clangFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "lld.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", lldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "llvm-ar.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", llvmArFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    BuildCache &mBuildCache;
};
}
