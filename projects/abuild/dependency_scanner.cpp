#ifdef _MSC_VER
export module abuild : dependency_scanner;

import : build_cache;
import : tokenizer;
#endif

namespace abuild
{
class DependencyScanner
{
public:
    explicit DependencyScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        for (auto it = cache["sources"].MemberBegin(); it != cache["sources"].MemberEnd(); ++it)
        {
            std::cout << it->name.GetString() << '\n';
            const std::string content = readFile(it->name.GetString());

            Tokenizer tokenizer{content};

            for (Token token = tokenizer.next(); !token.name.empty(); token = tokenizer.next())
            {
                std::cout << "  " << token.moduleName << (token.moduleName.empty() ? "" : ":") << token.name << '\n';
            }
        }
    }

private:
    [[nodiscard]] auto readFile(const std::filesystem::path &path) -> std::string
    {
        std::ifstream file{path};
        std::string data;

        file.seekg(0, std::ios::end);
        data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        data.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

        return data;
    }

    BuildCache &mBuildCache;
};
}
