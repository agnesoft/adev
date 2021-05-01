import atest;
import abuild;
import abuild_test_cache;

using atest::expect;
using atest::suite;
using atest::test;

class SettingsMatcher : public atest::MatcherBase
{
public:
    [[nodiscard]] auto actual(const std::string &left, [[maybe_unused]] const std::string &right) const -> std::string
    {
        rapidjson::Document doc;
        doc.Parse(left.c_str());
        std::vector<std::string> vec;

        if (doc.HasMember("settings"))
        {
            for (rapidjson::Value::MemberIterator setting = doc["settings"].MemberBegin(); setting != doc["settings"].MemberEnd(); ++setting)
            {
                vec.push_back(setting->name.GetString());
            }
        }

        return atest::stringify(vec);
    }

    [[nodiscard]] auto operator()(const std::string &left, const std::string &right) const -> bool
    {
        rapidjson::Document doc;
        doc.Parse(left.c_str());

        return doc.HasMember("settings")
            && doc["settings"].HasMember(right);
    }
};

static const auto testSuite = suite("abuild::Settings", [] {
    test("set", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::Settings{cache};
        }

        expect(testCache.content()).toMatch<SettingsMatcher>("cppSourceExtensions");
        expect(testCache.content()).toMatch<SettingsMatcher>("cppHeaderExtensions");
        expect(testCache.content()).toMatch<SettingsMatcher>("ignoreDirectories");
        expect(testCache.content()).toMatch<SettingsMatcher>("skipDirectories");
        expect(testCache.content()).toMatch<SettingsMatcher>("squashDirectories");
        expect(testCache.content()).toMatch<SettingsMatcher>("testDirectories");
        expect(testCache.content()).toMatch<SettingsMatcher>("projectNameSeparator");
    });
});
