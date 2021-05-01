import atest;
import abuild;
import abuild_test_cache;

using atest::expect;
using atest::suite;
using atest::test;

class ToolchainsMatcher : public atest::MatcherBase
{
public:
    [[nodiscard]] auto actual(const std::string &left, [[maybe_unused]] const std::string &right) const -> std::string
    {
        rapidjson::Document doc;
        doc.Parse(left.c_str());
        std::vector<std::string> vec;

        if (doc.HasMember("toolchains") && doc["toolchains"].MemberCount() > 1)
        {
            rapidjson::Value::MemberIterator toolchain = doc["toolchains"].MemberBegin();

            for (rapidjson::Value::MemberIterator property = toolchain->value.MemberBegin(); property != toolchain->value.MemberEnd(); ++property)
            {
                vec.push_back(property->name.GetString());
            }
        }

        return atest::stringify(vec);
    }

    [[nodiscard]] auto operator()(const std::string &left, const std::string &right) const -> bool
    {
        rapidjson::Document doc;
        doc.Parse(left.c_str());

        return doc.HasMember("toolchains")
            && doc["toolchains"].MemberCount() > 1
            && doc["toolchains"].MemberBegin()->value.HasMember(right);
    }
};

static const auto testSuite = suite("abuild::ToolchainScanner", [] {
    test("scan", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::ToolchainScanner{cache};
        }

        expect(testCache.content()).toMatch<ToolchainsMatcher>("type");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("compiler");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("compilerFlags");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("linker");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("linkerFlags");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("archiver");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("archiverFlags");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("ifc");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("include");
        expect(testCache.content()).toMatch<ToolchainsMatcher>("lib");
    });
});
