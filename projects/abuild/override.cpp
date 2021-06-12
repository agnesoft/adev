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
            applyExecutableFilenames(settings);
            applyIgnoreDirectories(settings);
            applyProjectNameSeparator(settings);
            applySkipDirectories(settings);
            applySquashDirectories(settings);
            applyTestDirectories(settings);
        }
    }

private:
    auto applyCppHeaderExtensions(Settings *settings) -> void
    {
        if (hasArray("settings", "cppHeaderExtensions"))
        {
            settings->setCppHeaderExtensions(values("settings", "cppHeaderExtensions"));
        }
    }

    auto applyCppSourceExtensions(Settings *settings) -> void
    {
        if (hasArray("settings", "cppSourceExtensions"))
        {
            settings->setCppSourceExtensions(values("settings", "cppSourceExtensions"));
        }
    }

    auto applyExecutableFilenames(Settings *settings) -> void
    {
        if (hasArray("settings", "executableFilenames"))
        {
            settings->setExecutableFilenames(values("settings", "executableFilenames"));
        }
    }

    auto applyIgnoreDirectories(Settings *settings) -> void
    {
        if (hasArray("settings", "ignoreDirectories"))
        {
            settings->setIgnoreDirectories(values("settings", "ignoreDirectories"));
        }
    }

    auto applyProjectNameSeparator(Settings *settings) -> void
    {
        if (hasString("settings", "projectNameSeparator"))
        {
            settings->setProjectNameSeparator(value("settings", "projectNameSeparator"));
        }
    }

    auto applySkipDirectories(Settings *settings) -> void
    {
        if (hasArray("settings", "skipDirectories"))
        {
            settings->setSkipDirectories(values("settings", "skipDirectories"));
        }
    }

    auto applySquashDirectories(Settings *settings) -> void
    {
        if (hasArray("settings", "squashDirectories"))
        {
            settings->setSquashDirectories(values("settings", "squashDirectories"));
        }
    }

    auto applyTestDirectories(Settings *settings) -> void
    {
        if (hasArray("settings", "testDirectories"))
        {
            settings->setTestDirectories(values("settings", "testDirectories"));
        }
    }

    [[nodiscard]] auto hasArray(const char *parent, const char *name) const -> bool
    {
        return mData[parent].HasMember(name) && mData[parent][name].IsArray();
    }

    [[nodiscard]] auto hasString(const char *parent, const char *name) const -> bool
    {
        return mData[parent].HasMember(name) && mData[parent][name].IsString();
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

    [[nodiscard]] auto value(const char *parent, const char *name) const -> std::string
    {
        return mData[parent][name].GetString();
    }

    [[nodiscard]] auto values(const char *parent, const char *name) const -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> vals;

        for (const rapidjson::Value &value : mData[parent][name].GetArray())
        {
            vals.insert(value.GetString());
        }

        return vals;
    }

    rapidjson::Document mData;
};
}
