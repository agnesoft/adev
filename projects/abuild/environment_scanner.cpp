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
    [[nodiscard]] auto clangArchiverFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
    }

    [[nodiscard]] auto clangCompilerFlags() -> rapidjson::Value
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

    [[nodiscard]] auto clangLinkerFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        return flags;
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

    [[nodiscard]] auto msvcArchiverFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/NOLOGO", mBuildCache.allocator()); //No archiver chitchat
        return flags;
    }

    [[nodiscard]] auto msvcCompilerFlags() -> rapidjson::Value
    {
        rapidjson::Value flags{rapidjson::kArrayType};
        flags.PushBack("/nologo", mBuildCache.allocator()); //No compiler chit chat
        flags.PushBack("/std:c++latest", mBuildCache.allocator()); //C++20 and beyond
        flags.PushBack("/W4", mBuildCache.allocator()); //Highest warning level
        flags.PushBack("/WX", mBuildCache.allocator()); //Warnings are errors
        flags.PushBack("/EHsc", mBuildCache.allocator()); //Enable C++ exceptions
        return flags;
    }

    [[nodiscard]] auto msvcLinkerFlags() -> rapidjson::Value
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
        toolchain.AddMember("compilerFlags", msvcCompilerFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "link.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", msvcLinkerFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "lib.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", msvcArchiverFlags(), mBuildCache.allocator());
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
#ifdef _MSC_VER
        scanWindows();
#else
        scanLinux();
#endif
    }

    auto scanLinux() -> void
    {
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
        toolchain.AddMember("compilerFlags", clangCompilerFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "lld.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", clangLinkerFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "llvm-ar.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", clangArchiverFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    BuildCache &mBuildCache;
};
}
