namespace abuild
{
class ToolchainsScanner
{
public:
    explicit ToolchainsScanner(BuildCache &cache) :
    	mBuildCache{cache}
    {
        detectGCC();
        detectClang();
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

    [[nodiscard]] auto clangToolchain(const std::string &version) -> rapidjson::Value
    {
        const std::filesystem::path binPath = "/usr/bin";

        rapidjson::Value toolchain{rapidjson::kObjectType};
        toolchain.AddMember("name", rapidjson::Value{"clang" + version, mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("compiler", rapidjson::Value{(binPath / ("clang++-" + version)).lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("compilerFlags", clangFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", rapidjson::Value{(binPath / "ld").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", rapidjson::Value{(binPath / "ar").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", arFlags(), mBuildCache.allocator());
        toolchain.AddMember("ifc", "", mBuildCache.allocator());
        toolchain.AddMember("include", "", mBuildCache.allocator());
        toolchain.AddMember("lib", "", mBuildCache.allocator());
        return toolchain;
    }

    auto detectGCC() -> void
    {
        detectGCCVersion("9");
        detectGCCVersion("10");
        detectGCCVersion("11");
    }

    auto detectGCCVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/g++-" + version))
        {
            mBuildCache["toolchains"].AddMember(rapidjson::Value{"gcc" + version, mBuildCache.allocator()}, gccToolchain(version), mBuildCache.allocator());
        }
    }

    auto detectClang() -> void
    {
        detectClangVersion("11");
        detectClangVersion("12");
    }

    auto detectClangVersion(const std::string &version) -> void
    {
        if (std::filesystem::exists("/usr/bin/clang++-" + version))
        {
            mBuildCache["toolchains"].AddMember(rapidjson::Value{"clang" + version, mBuildCache.allocator()}, clangToolchain(version), mBuildCache.allocator());
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
        toolchain.AddMember("compilerFlags", gccFlags(), mBuildCache.allocator());
        toolchain.AddMember("linker", rapidjson::Value{(binPath / "ld").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("linkerFlags", ldFlags(), mBuildCache.allocator());
        toolchain.AddMember("archiver", rapidjson::Value{(binPath / "ar").lexically_normal().string(), mBuildCache.allocator()}, mBuildCache.allocator());
        toolchain.AddMember("archiverFlags", arFlags(), mBuildCache.allocator());
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

    auto scanLinux() -> void
    {
        detectGCC();
        detectClang();
    }
    
    BuildCache &mBuildCache;
};
}
