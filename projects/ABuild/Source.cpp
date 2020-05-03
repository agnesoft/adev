#include "Source.hpp"

#include <clang-c/Index.h>

#include <functional>

namespace abuild
{
namespace libclang
{
class TranslationUnit
{
public:
    TranslationUnit(const std::filesystem::path &path) :
        mIndex{clang_createIndex(0, 0)},
        mUnit{clang_parseTranslationUnit(mIndex,
                                         path.string().c_str(),
                                         nullptr,
                                         0,
                                         nullptr,
                                         0,
                                         static_cast<int>(static_cast<unsigned int>(CXTranslationUnit_Incomplete)
                                                          | static_cast<unsigned int>(CXTranslationUnit_DetailedPreprocessingRecord)
                                                          | static_cast<unsigned int>(CXTranslationUnit_SkipFunctionBodies)
                                                          | static_cast<unsigned int>(CXTranslationUnit_SingleFileParse)
                                                          | static_cast<unsigned int>(CXTranslationUnit_RetainExcludedConditionalBlocks)
                                                          | static_cast<unsigned int>(CXTranslationUnit_KeepGoing)))}
    {
    }

    TranslationUnit(const TranslationUnit &other) = delete;
    TranslationUnit(TranslationUnit &&other) = delete;

    ~TranslationUnit()
    {
        clang_disposeTranslationUnit(mUnit);
        clang_disposeIndex(mIndex);
    }

    [[nodiscard]] auto unit() const noexcept -> CXTranslationUnit
    {
        return mUnit;
    }

    auto operator=(const TranslationUnit &other) -> TranslationUnit = delete;
    auto operator=(TranslationUnit &&other) -> TranslationUnit = delete;

private:
    CXIndex mIndex = nullptr;
    CXTranslationUnit mUnit = nullptr;
};

class String
{
public:
    explicit String(CXString string) :
        mString{string}
    {
    }

    String(const String &other) = delete;
    String(String &&other) = delete;

    ~String()
    {
        clang_disposeString(mString);
    }

    [[nodiscard]] auto c_str() const -> const char *
    {
        return clang_getCString(mString);
    }

    auto operator=(const String &other) = delete;
    auto operator=(String &&other) = delete;

private:
    CXString mString;
};
}

Source::Source(const std::filesystem::path &path) :
    File{path}
{
    static_cast<void>(update());
}

auto Source::includes() const noexcept -> const std::vector<std::string> &
{
    return mIncludes;
}

auto Source::update() -> bool
{
    if (mLastWriteTime != lastWriteTime())
    {
        mLastWriteTime = lastWriteTime();
        return scanContent();
    }

    return false;
}

auto Source::extractIncludes() -> std::vector<std::string>
{
    const libclang::TranslationUnit translationUnit{path()};
    std::vector<std::string> includedPaths;

    if (translationUnit.unit() != nullptr)
    {
        const auto includeVisitor = [](CXCursor cursor, [[maybe_unused]] CXCursor parent, CXClientData data) {
            if (clang_getCursorKind(cursor) == CXCursor_InclusionDirective)
            {
                static_cast<std::vector<std::string> *>(data)->push_back(libclang::String{clang_getCursorSpelling(cursor)}.c_str());
            }
            return CXChildVisit_Continue;
        };

        clang_visitChildren(clang_getTranslationUnitCursor(translationUnit.unit()), includeVisitor, &includedPaths);
    }

    return includedPaths;
}

auto Source::lastWriteTime() const -> std::filesystem::file_time_type
{
    return std::filesystem::last_write_time(path());
}

auto Source::scanContent() -> bool
{
    const std::size_t hash = std::hash<std::string>{}(content());

    if (hash != mHash)
    {
        mHash = hash;
        mIncludes = extractIncludes();
        return true;
    }

    return false;
}
}
