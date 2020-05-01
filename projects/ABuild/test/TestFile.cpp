#include "TestFile.hpp"

#include <Catch2/catch.hpp>

namespace abuildtest
{

TestFile::TestFile(const std::string &name) :
    mFile{name}
{
    removeFile();
}

TestFile::TestFile(const std::string &name, const std::string &content) :
    mFile{name}
{
    std::fstream stream{mFile.string(), testFileOpenMode()};
    stream.write(content.data(), content.size());
}

TestFile::~TestFile()
{
    try
    {
        try
        {
            removeFile();
        }
        catch (...)
        {
            INFO("Unable to remove file '" + mFile.string() + "' in abuildtest::TestFile::~TestFile()");
        }
    }
    catch (...)
    {
        std::terminate();
    }
}

auto TestFile::path() const noexcept -> const std::filesystem::path &
{
    return mFile;
}

void TestFile::removeFile() const
{
    if (std::filesystem::exists(mFile))
    {
        std::filesystem::remove(mFile);
    }
}

auto testFileOpenMode() -> std::fstream::openmode
{
    return static_cast<std::fstream::openmode>(static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc));
}
}
