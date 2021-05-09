#ifdef _MSC_VER
export module abuild : build_cache;
import acore;
import "rapidjson.hpp";
#endif

namespace abuild
{
export class BuildCache
{
public:
    explicit BuildCache(const std::filesystem::path &cacheFile) :
        mFilePath{cacheFile}
    {
        load();
    }

    BuildCache(const BuildCache &other) = delete;
    BuildCache(BuildCache &&other) noexcept = default;

    ~BuildCache()
    {
        try
        {
            save();
        }
        catch (...)
        {
        }
    }

    [[nodiscard]] auto allocator() -> rapidjson::Document::AllocatorType &
    {
        return mData.GetAllocator();
    }

    [[nodiscard]] auto cppHeaderExtensions() const -> const rapidjson::Value &
    {
        return mData["settings"]["cpp_header_extensions"];
    }

    [[nodiscard]] auto cppSourceExtensions() const -> const rapidjson::Value &
    {
        return mData["settings"]["cpp_source_extensions"];
    }

    [[nodiscard]] auto ignoreDirectories() const -> const rapidjson::Value &
    {
        return mData["settings"]["ignore_directories"];
    }

    [[nodiscard]] auto headers() -> rapidjson::Value &
    {
        return mData["headers"];
    }

    [[nodiscard]] auto modules() -> rapidjson::Value &
    {
        return mData["modules"];
    }

    [[nodiscard]] auto projectNameSeparator() const -> const rapidjson::Value &
    {
        return mData["settings"]["project_name_separator"];
    }

    [[nodiscard]] auto projects() -> rapidjson::Value &
    {
        return mData["projects"];
    }

    [[nodiscard]] auto settings() -> rapidjson::Value &
    {
        return mData["settings"];
    }

    [[nodiscard]] auto skipDirectories() const -> const rapidjson::Value &
    {
        return mData["settings"]["skip_directories"];
    }

    [[nodiscard]] auto sources() -> rapidjson::Value &
    {
        return mData["sources"];
    }

    [[nodiscard]] auto squashDirectories() const -> const rapidjson::Value &
    {
        return mData["settings"]["squash_directories"];
    }

    [[nodiscard]] auto testDirectories() const -> const rapidjson::Value &
    {
        return mData["settings"]["test_directories"];
    }

    [[nodiscard]] auto toolchains() -> rapidjson::Value &
    {
        return mData["toolchains"];
    }

    auto operator=(const BuildCache &other) -> BuildCache & = delete;
    auto operator=(BuildCache &&other) noexcept -> BuildCache & = default;

private:
    auto asArray(const std::vector<std::string> &values) -> rapidjson::Value
    {
        rapidjson::Value ar{rapidjson::kArrayType};

        for (const std::string &value : values)
        {
            ar.PushBack(rapidjson::Value{value, mData.GetAllocator()}, mData.GetAllocator());
        }

        return ar;
    }

    auto load() -> void
    {
        if (std::filesystem::exists(mFilePath))
        {
            std::string cache = readCache();
            mData.Parse(cache.c_str());
        }
        else
        {
            mData.SetObject();
            mData.AddMember("headers", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
            mData.AddMember("modules", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
            mData.AddMember("projects", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
            mData.AddMember("sources", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
            mData.AddMember("settings", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
            mData["settings"].AddMember("cpp_source_extensions", asArray({".cpp", ".cxx", ".cc", ".ixx"}), mData.GetAllocator());
            mData["settings"].AddMember("cpp_header_extensions", asArray({".hpp", ".hxx", ".h"}), mData.GetAllocator());
            mData["settings"].AddMember("ignore_directories", asArray({"build"}), mData.GetAllocator());
            mData["settings"].AddMember("skip_directories", asArray({"projects", "Projects"}), mData.GetAllocator());
            mData["settings"].AddMember("squash_directories", asArray({"src", "srcs", "SRC", "Src", "source", "sources", "Source", "Sources", "include", "Include", "includes", "Includes"}), mData.GetAllocator());
            mData["settings"].AddMember("test_directories", asArray({"test", "Test", "tests", "Tests"}), mData.GetAllocator());
            mData["settings"].AddMember("project_name_separator", ".", mData.GetAllocator());
            mData.AddMember("toolchains", rapidjson::Value{rapidjson::kObjectType}, mData.GetAllocator());
        }
    }

    [[nodiscard]] auto readCache() -> std::string
    {
        std::ifstream file{mFilePath};
        std::string data;

        file.seekg(0, std::ios::end);
        data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        data.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

        return data;
    }

    auto save() -> void
    {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{buffer};
        mData.Accept(writer);
        writeCache(buffer.GetString(), buffer.GetSize());
    }

    auto writeCache(const char *data, size_t size) -> void
    {
        std::filesystem::create_directory(mFilePath.parent_path());
        std::ofstream file{mFilePath};
        file.write(data, size);
    }

    std::filesystem::path mFilePath;
    rapidjson::Document mData;
};
}
