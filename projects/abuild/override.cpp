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

            if (std::filesystem::is_regular_file(path) && (path.filename().string() == ".abuild" || path.extension().string() == ".abuild"))
            {
                const std::string data = readFile(path);
                mData.Parse(data.c_str());
            }
        }
    }

    auto applyOverride(Settings *settings) -> void
    {
        if (mData.IsObject() && mData.HasMember("settings"))
        {
            applyCppHeaderExtensions(settings);
            applyCppSourceExtensions(settings);
        }
    }

private:
    auto applyCppHeaderExtensions(Settings *settings) -> void
    {
        if (hasCppHeaderExtensions())
        {
            settings->setCppHeaderExtensions(values("cppHeaderExtensions"));
        }
    }

    auto applyCppSourceExtensions(Settings *settings) -> void
    {
        if (hasCppSourceExtensions())
        {
            settings->setCppSourceExtensions(values("cppSourceExtensions"));
        }
    }

    [[nodiscard]] auto hasCppHeaderExtensions() const -> bool
    {
        return mData["settings"].HasMember("cppHeaderExtensions") && mData["settings"]["cppHeaderExtensions"].IsArray();
    }

    [[nodiscard]] auto hasCppSourceExtensions() const -> bool
    {
        return mData["settings"].HasMember("cppSourceExtensions") && mData["settings"]["cppSourceExtensions"].IsArray();
    }

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

    [[nodiscard]] auto values(const char *name) const -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> vals;

        for (const rapidjson::Value &value : mData["settings"][name].GetArray())
        {
            vals.insert(value.GetString());
        }

        return vals;
    }

    rapidjson::Document mData;
};
}
