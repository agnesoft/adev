#ifdef _MSC_VER
export module abuild : code_scanner;
import : build_cache;
import : tokenizer;
#endif

namespace abuild
{
export class CodeScanner
{
public:
    explicit CodeScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        processFiles(mBuildCache.sources());
        processFiles(mBuildCache.headers());
    }

private:
    [[nodiscard]] auto moduleValue(const std::string &name) -> rapidjson::Value &
    {
        if (!mBuildCache.modules().HasMember(name))
        {
            mBuildCache.modules().AddMember(rapidjson::Value{name, mBuildCache.allocator()}, rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
            mBuildCache.modules()[name].AddMember("partitions", rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
        }

        return mBuildCache.modules()[name];
    }

    auto processFile(rapidjson::Value::MemberIterator &file) -> void
    {
        const std::string content = readFile(file->name.GetString());

        Tokenizer tokenizer{content};

        while (const auto token = tokenizer.next())
        {
            processToken(file, *token);
        }
    }

    auto processFiles(rapidjson::Value &files) -> void
    {
        for (rapidjson::Value::MemberIterator it = files.MemberBegin(); it != files.MemberEnd(); ++it)
        {
            processFile(it);
        }
    }

    auto addDependency(rapidjson::Value &val, const std::string &name) -> void
    {
        val.AddMember(rapidjson::Value(name, mBuildCache.allocator()), rapidjson::Value{rapidjson::kStringType}, mBuildCache.allocator());
    }

    auto addModule(rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        addDependency(value(file, "module"), token.name);
        rapidjson::Value &mod = moduleValue(token.name);
        mod.AddMember("file", rapidjson::Value{file->name, mBuildCache.allocator()}, mBuildCache.allocator());
        mod.AddMember("exported", token.exported, mBuildCache.allocator());
    }

    auto addModulePartition(rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        addDependency(value(file, "module_partitions"), token.name);
        addDependency(value(file, "module"), token.moduleName);
        rapidjson::Value &partitions = moduleValue(token.moduleName)["partitions"];
        partitions.AddMember(rapidjson::Value{token.name, mBuildCache.allocator()}, rapidjson::Value{file->name, mBuildCache.allocator()}, mBuildCache.allocator());
    }

    auto processToken(rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        switch (token.type)
        {
        case Token::Type::IncludeLocal:
            addDependency(value(file, "includes_local"), token.name);
            break;
        case Token::Type::IncludeExternal:
            addDependency(value(file, "includes_external"), token.name);
            break;
        case Token::Type::Module:
            addModule(file, token);
            break;
        case Token::Type::ModulePartition:
            addModulePartition(file, token);
            break;
        case Token::Type::ImportModule:
            addDependency(value(file, "import_modules"), token.name);
            break;
        case Token::Type::ImportIncludeLocal:
            addDependency(value(file, "import_includes_local"), token.name);
            break;
        case Token::Type::ImportIncludeExternal:
            addDependency(value(file, "import_includes_external"), token.name);
            break;
        case Token::Type::ImportModulePartition:
            addDependency(value(file, "import_module_partitions"), token.name);
            break;
        case Token::Type::None:
            throw std::runtime_error{"Unknown token type for token '" + token.name + '\''};
        }
    }

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

    auto value(rapidjson::Value::MemberIterator &file, const char *valueName) -> rapidjson::Value &
    {
        if (!file->value.HasMember(valueName))
        {
            file->value.AddMember(rapidjson::Value{valueName, mBuildCache.allocator()}, rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
        }

        return file->value[valueName];
    }

    BuildCache &mBuildCache;
};
}
