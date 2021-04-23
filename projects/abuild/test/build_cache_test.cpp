import atest;
import abuild;

import "rapidjson.hpp";

using atest::expect;
using atest::suite;
using atest::test;

class TestCache
{
public:
    TestCache()
    {
        removeFile();
    }

    TestCache(const std::string &data)
    {
        std::filesystem::create_directory(mFilePath.parent_path());
        std::ofstream file{mFilePath};
        std::string buffer = json(data);
        file.write(buffer.c_str(), buffer.size());
    }

    TestCache(TestCache &other) = delete;
    TestCache(TestCache &&other) noexcept = default;

    ~TestCache()
    {
        try
        {
            try
            {
                removeFile();
            }
            catch (std::exception &e)
            {
                std::cout << "Failed to remove file '" << mFilePath << ": " << e.what() << '\n';
            }
        }
        catch (...)
        {
        }
    }

    [[nodiscard]] auto content() const -> std::string
    {
        std::ifstream file{mFilePath};
        std::string data;

        file.seekg(0, std::ios::end);
        data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        data.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

        return data;
    }

    [[nodiscard]] static auto json(const std::string &data) -> std::string
    {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{buffer};
        rapidjson::Document doc;
        doc.Parse(data.c_str());
        doc.Accept(writer);
        return std::string(buffer.GetString(), buffer.GetSize());
    }

    [[nodiscard]] auto exists() -> bool
    {
        return std::filesystem::exists(mFilePath);
    }

    auto operator=(TestCache &other) -> TestCache & = delete;
    auto operator=(TestCache &&other) noexcept -> TestCache & = default;

private:
    auto removeFile() -> void
    {
        std::filesystem::remove(mFilePath);
    }

    std::filesystem::path mFilePath = std::filesystem::current_path() / "build" / ".abuild";
};

static const auto testSuite = suite("abuild::BuildCache", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::BuildCache>).toBe(true);
        expect(std::is_copy_constructible_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_constructible_v<abuild::BuildCache>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::BuildCache>).toBe(true);
    });

    test("missing cache", [] {
        TestCache testCache;

        expect(testCache.exists()).toBe(false);

        {
            abuild::BuildCache cache;
        }

        expect(testCache.exists()).toBe(true);
        expect(testCache.content()).toBe(TestCache::json("{}"));
    });

    test("existing cache", [] {
        TestCache testCache{"{ \"Value\": 1 }"};

        expect(testCache.exists()).toBe(true);

        {
            abuild::BuildCache cache;
        }

        expect(testCache.content()).toBe(TestCache::json("{ \"Value\": 1 }"));
    });
});
