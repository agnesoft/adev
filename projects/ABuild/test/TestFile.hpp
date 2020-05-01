#ifndef ABUILD_TEST_TEST_FILE_HPP
#define ABUILD_TEST_TEST_FILE_HPP

#include <filesystem>
#include <fstream>

namespace abuildtest
{
[[nodiscard]] auto testFileOpenMode() -> std::fstream::openmode;

class TestFile
{
public:
    explicit TestFile(const std::string &name);
    TestFile(const std::string &name, const std::string &content);
    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = default;
    ~TestFile();

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) -> TestFile & = default;

private:
    void removeFile() const;

    std::filesystem::path mFile;
};
}

#endif
