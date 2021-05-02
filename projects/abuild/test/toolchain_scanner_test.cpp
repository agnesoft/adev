import atest;
import abuild;
import abuild_test_cache;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

namespace
{
[[nodiscard]] auto asVector(const rapidjson::Value &ar) -> std::vector<std::string>
{
    std::vector<std::string> values;

    for (const rapidjson::Value &value : ar.GetArray())
    {
        values.push_back(value.GetString());
    }

    return values;
}
}

static const auto testSuite = suite("abuild::ToolchainScanner", [] {
    test("scan", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::ToolchainScanner{cache};
        }

        rapidjson::Document doc;
        doc.Parse(testCache.content().c_str());

        assert_(doc.HasMember("toolchains")).toBe(true);
        assert_(doc["toolchains"].IsObject()).toBe(true);

        for (auto toolchain = doc["toolchains"].MemberBegin(); toolchain != doc["toolchains"].MemberEnd(); ++toolchain)
        {
            expect(toolchain->value.HasMember("type")).toBe(true);
            expect(toolchain->value.HasMember("compiler")).toBe(true);
            expect(toolchain->value.HasMember("compiler_flags")).toBe(true);
            expect(toolchain->value.HasMember("linker")).toBe(true);
            expect(toolchain->value.HasMember("linker_flags")).toBe(true);
            expect(toolchain->value.HasMember("archiver")).toBe(true);
            expect(toolchain->value.HasMember("archiver_flags")).toBe(true);
            expect(toolchain->value.HasMember("ifc")).toBe(true);
            expect(toolchain->value.HasMember("include")).toBe(true);
            expect(toolchain->value.HasMember("lib")).toBe(true);
        }
    });

    test("compiler_flags", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::ToolchainScanner{cache};
        }

        rapidjson::Document doc;
        doc.Parse(testCache.content().c_str());

        assert_(doc.HasMember("toolchains")).toBe(true);
        assert_(doc["toolchains"].IsObject()).toBe(true);

        for (auto toolchain = doc["toolchains"].MemberBegin(); toolchain != doc["toolchains"].MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("compiler_flags")).toBe(true);
            assert_(toolchain->value["compiler_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["compiler_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/nologo",
                                                            "/std:c++latest",
                                                            "/W4",
                                                            "/WX",
                                                            "/EHsc"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{"-std=c++20",
                                                            "-Wall",
                                                            "-Wextra",
                                                            "-pedantic",
                                                            "-Werror",
                                                            "-fmodules"});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{"-std=c++20",
                                                            "-Wall",
                                                            "-Wextra",
                                                            "-pedantic",
                                                            "-Werror"});
            }
        }
    });

    test("linker_flags", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::ToolchainScanner{cache};
        }

        rapidjson::Document doc;
        doc.Parse(testCache.content().c_str());

        assert_(doc.HasMember("toolchains")).toBe(true);
        assert_(doc["toolchains"].IsObject()).toBe(true);

        for (auto toolchain = doc["toolchains"].MemberBegin(); toolchain != doc["toolchains"].MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("linker_flags")).toBe(true);
            assert_(toolchain->value["linker_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["linker_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/WX"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
        }
    });

    test("archiver_flags", [] {
        TestCache testCache;

        {
            abuild::BuildCache cache;
            abuild::ToolchainScanner{cache};
        }

        rapidjson::Document doc;
        doc.Parse(testCache.content().c_str());

        assert_(doc.HasMember("toolchains")).toBe(true);
        assert_(doc["toolchains"].IsObject()).toBe(true);

        for (auto toolchain = doc["toolchains"].MemberBegin(); toolchain != doc["toolchains"].MemberEnd(); ++toolchain)
        {
            assert_(toolchain->value.HasMember("type")).toBe(true);
            assert_(toolchain->value.HasMember("archiver_flags")).toBe(true);
            assert_(toolchain->value["archiver_flags"].IsArray()).toBe(true);

            const std::string type = toolchain->value["type"].GetString();
            const std::vector<std::string> flags = asVector(toolchain->value["archiver_flags"]);

            if (type == "msvc")
            {
                expect(flags).toBe(std::vector<std::string>{"/NOLOGO"});
            }
            else if (type == "clang")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
            else if (type == "gcc")
            {
                expect(flags).toBe(std::vector<std::string>{});
            }
        }
    });
});
