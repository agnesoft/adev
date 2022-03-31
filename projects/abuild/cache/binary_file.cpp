#ifndef __clang__
export module abuild.cache:binary_file;
export import :file;
#endif

namespace abuild
{
export struct TranslationUnit;

export struct BinaryFile : File
{
    std::vector<TranslationUnit *> translationUnits;
};
}
