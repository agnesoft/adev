#ifndef __clang__
export module abuild.cache:linked_file;
export import :file;
export import :project;
#endif

namespace abuild
{
export struct TranslationUnit;

export struct LinkedFile : File
{
    Project *project = nullptr;
    std::vector<TranslationUnit *> translationUnits;
};
}
