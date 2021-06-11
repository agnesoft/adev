#ifdef _MSC_VER
export module abuild : abuild_override;
export import : settings;
import<rapidjson.hpp>;
#endif

namespace abuild
{
export class Override
{
public:
    Override(const std::filesystem::path &projectRoot)
    {
        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(projectRoot))
        {
            const std::filesystem::path &path = entry.path();

            if (std::filesystem::is_regular_file(path) && path.extension().string() == ".abuild")
            {
                const std::string data = readFile(path);
                mData.Parse(data.c_str());
            }
        }
    }

    auto applyOverride([[maybe_unused]] Settings *settings) -> void
    {
        if (mData.IsObject())
        {
        }
    }

private:
    [[nodiscard]] auto readFile(const std::filesystem::path &path) -> std::string
    {
        std::ifstream file{path};
        std::string data;

        file.seekg(0, std::ios::end);
        data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        data.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

        return data;
    }

    rapidjson::Document mData;
};
}
