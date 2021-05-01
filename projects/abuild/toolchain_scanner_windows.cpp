#ifdef _MSC_VER
export module abuild : toolchain_scanner_windows;

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
        detectMSVC();
        detectClang();
    }

private:
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

    [[nodiscard]] auto clangToolchain(const std::filesystem::path &path) -> rapidjson::Value
    {
        const std::filesystem::path binPath = path / "bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("type", "clang", mBuildCache.allocator());
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

    auto detectClang() -> void
    {
        const std::filesystem::path clangRoot = "C:/Program Files/LLVM/";

        if (std::filesystem::exists(clangRoot))
        {
            mBuildCache["toolchains"].AddMember("clang", clangToolchain(clangRoot), mBuildCache.allocator());
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

    [[nodiscard]] auto
        libFlags() -> rapidjson::Value
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
        toolchain.AddMember("compilerFlags", clFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", (binPath / "link.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", linkFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", (binPath / "lib.exe").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", libFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", (path / "ifc").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("include", (path / "include").lexically_normal().string(), mBuildCache.allocator());
        toolchain.AddMember("lib", (path / "lib" / "x64").lexically_normal().string(), mBuildCache.allocator());
        return toolchain;
    }

    BuildCache &mBuildCache;
};
}
