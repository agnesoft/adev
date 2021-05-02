export module abuild_test_utilities;

export import atest;

#ifdef _MSC_VER
export import : test_cache;
export import : test_project;
#else
// clang-format off
import "rapidjson.hpp";
#include "test_cache.cpp"
#include "test_project.cpp"
// clang-format on
#endif

export class OneOfMatcher : public atest::MatcherBase
{
public:
    [[nodiscard]] auto operator()(const std::vector<std::string> &left, const std::string &right) const -> bool
    {
        return std::find(left.begin(), left.end(), right) != left.end();
    }
};

export [[nodiscard]] auto asVector(const rapidjson::Value &data) -> std::vector<std::string>
{
    std::vector<std::string> values;

    if (data.IsArray())
    {
        for (const rapidjson::Value &value : data.GetArray())
        {
            values.push_back(value.GetString());
        }
    }
    else if (data.IsObject())
    {
        for (auto it = data.MemberBegin(); it != data.MemberEnd(); ++it)
        {
            values.push_back(it->name.GetString());
        }
    }

    return values;
}
