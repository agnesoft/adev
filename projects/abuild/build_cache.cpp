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
    BuildCache()
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

    [[nodiscard]] auto data() noexcept -> rapidjson::Document &
    {
        return mData;
    }

    auto operator=(const BuildCache &other) -> BuildCache & = delete;
    auto operator=(BuildCache &&other) noexcept -> BuildCache & = default;

private:
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

    std::filesystem::path mFilePath = std::filesystem::current_path() / "build" / ".abuild";
    rapidjson::Document mData;
};
}
