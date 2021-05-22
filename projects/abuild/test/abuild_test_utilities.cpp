export module abuild_test_utilities;

export import abuild;
export import atest;

#ifdef _MSC_VER
export import : test_cache;
export import : test_project;
export import : test_project_with_content;
#else
// clang-format off
export import "rapidjson.hpp";
#include "test_cache.cpp"
#include "test_project.cpp"
#include "test_project_with_content.cpp"
// clang-format on
#endif

namespace std
{
export auto operator<<(std::ostream &stream, const abuild::ModuleVisibility &visibility) -> std::ostream &
{
    switch (visibility)
    {
    case abuild::ModuleVisibility::Public:
        return stream << "Public";
    case abuild::ModuleVisibility::Private:
        return stream << "Private";
    }

    return stream;
}

export auto operator<<(std::ostream &stream, const abuild::DependencyVisibility &visibility) -> std::ostream &
{
    switch (visibility)
    {
    case abuild::DependencyVisibility::Public:
        return stream << "Public";
    case abuild::DependencyVisibility::Private:
        return stream << "Private";
    }

    return stream;
}
}

export [[nodiscard]] auto lastModified(const std::string &path) -> std::int64_t
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
}
