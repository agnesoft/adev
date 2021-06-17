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
            applyGCCInstallDirectory(settings);
            applyClangInstallDirectory(settings);
            applyMSVCInstallDirectory(settings);
        }
    }

private:
    auto applyClangInstallDirectory(Settings *settings) -> void
    {
        if (hasValidString("settings", "clangInstallDirectory"))
        {
            settings->setClangInstallDirectory(value("settings", "clangInstallDirectory"));
        }
    }

    auto applyCppHeaderExtensions(Settings *settings) -> void
    {
        if (hasValidArray("settings", "cppHeaderExtensions"))
        {
            settings->setCppHeaderExtensions(values("settings", "cppHeaderExtensions"));
        }
    }

    auto applyCppSourceExtensions(Settings *settings) -> void
    {
        if (hasValidArray("settings", "cppSourceExtensions"))
        {
            settings->setCppSourceExtensions(values("settings", "cppSourceExtensions"));
        }
    }

    auto applyExecutableFilenames(Settings *settings) -> void
    {
        if (hasValidArray("settings", "executableFilenames"))
        {
            settings->setExecutableFilenames(values("settings", "executableFilenames"));
        }
    }

    auto applyGCCInstallDirectory(Settings *settings) -> void
    {
        if (hasValidString("settings", "gccInstallDirectory"))
        {
            settings->setGCCInstallDirectory(value("settings", "gccInstallDirectory"));
        }
    }

    auto applyIgnoreDirectories(Settings *settings) -> void
    {
        if (hasValidArray("settings", "ignoreDirectories"))
        {
            settings->setIgnoreDirectories(values("settings", "ignoreDirectories"));
        }
    }

    auto applyMSVCInstallDirectory(Settings *settings) -> void
    {
        if (hasValidString("settings", "msvcInstallDirectory"))
        {
            settings->setMSVCInstallDirectory(value("settings", "msvcInstallDirectory"));
        }
    }

    auto applyProjectNameSeparator(Settings *settings) -> void
    {
        if (hasValidString("settings", "projectNameSeparator"))
        {
            settings->setProjectNameSeparator(value("settings", "projectNameSeparator"));
        }
    }

    auto applySkipDirectories(Settings *settings) -> void
    {
        if (hasValidArray("settings", "skipDirectories"))
        {
            settings->setSkipDirectories(values("settings", "skipDirectories"));
        }
    }

    auto applySquashDirectories(Settings *settings) -> void
    {
        if (hasValidArray("settings", "squashDirectories"))
        {
            settings->setSquashDirectories(values("settings", "squashDirectories"));
        }
    }

    auto applyTestDirectories(Settings *settings) -> void
    {
        if (hasValidArray("settings", "testDirectories"))
        {
            settings->setTestDirectories(values("settings", "testDirectories"));
        }
    }

    [[nodiscard]] auto hasValidArray(const char *parent, const char *name) const -> bool
    {
        return mData[parent].HasMember(name) && validateArray(parent, name);
    }

    [[nodiscard]] auto hasValidString(const char *parent, const char *name) const -> bool
    {
        return mData[parent].HasMember(name) && validateString(parent, name);
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

    auto validateArray(const char *parent, const char *name) const -> bool
    {
        if (!mData[parent][name].IsArray())
        {
            throw std::runtime_error{"Override error. Value of [\"" + std::string{parent} + "\"][\"" + std::string{name} + "\"] must be a list of strings."};
        }
        else
        {
            return true;
        }
    }

    auto validateArrayValue(const char *parent, const char *name, const rapidjson::Value &val) const -> void
    {
        if (!val.IsString())
        {
            throw std::runtime_error{"Override error. Value of [\"" + std::string{parent} + "\"][\"" + std::string{name} + "\"] must be a list of strings."};
        }
    }

    auto validateString(const char *parent, const char *name) const -> bool
    {
        if (!mData[parent][name].IsString())
        {
            throw std::runtime_error{"Override error. Value of [\"" + std::string{parent} + "\"][\"" + std::string{name} + "\"] must be a string."};
        }
        else
        {
            return true;
        }
    }

    [[nodiscard]] auto value(const char *parent, const char *name) const -> std::string
    {
        return mData[parent][name].GetString();
    }

    [[nodiscard]] auto values(const char *parent, const char *name) const -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> vals;

        for (const rapidjson::Value &val : mData[parent][name].GetArray())
        {
            validateArrayValue(parent, name, val);
            vals.insert(val.GetString());
        }

        return vals;
    }

    rapidjson::Document mData;
};
}
