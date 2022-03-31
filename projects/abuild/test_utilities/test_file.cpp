#ifndef __clang__
export module abuild.test_utilities:test_file;
export import astl;
#endif

namespace abuild
{
//! \private
export class TestFile
{
public:
    explicit TestFile(std::filesystem::path path) :
        filePath{std::move(path)}
    {
    }

    TestFile(std::filesystem::path path, std::string_view content) :
        filePath{std::move(path)}
    {
        std::ofstream{this->filePath} << content;
    }

    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) noexcept = default;

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return this->filePath;
    }

    ~TestFile()
    {
        std::error_code ec{};
        std::filesystem::remove(this->filePath, ec);
    }

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) noexcept -> TestFile & = default;

private:
    std::filesystem::path filePath;
};
}
