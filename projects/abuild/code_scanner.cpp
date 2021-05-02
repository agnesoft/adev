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
        processFiles(cache["sources"]);
        processFiles(cache["headers"]);
    }

private:
    [[nodiscard]] auto moduleValue(const std::string &name) -> rapidjson::Value &
    {
        if (!mBuildCache["modules"].HasMember(name))
        {
            mBuildCache["modules"].AddMember(rapidjson::Value{name, mBuildCache.allocator()}, rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
            mBuildCache["modules"][name].AddMember("partitions", rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
        }

        return mBuildCache["modules"][name];
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

    auto processIncludeImport(rapidjson::Value &val, const Token &token) -> void
    {
        val.PushBack(rapidjson::Value(token.name, mBuildCache.allocator()), mBuildCache.allocator());
    }

    auto processModule(const rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        rapidjson::Value &mod = moduleValue(token.name);
        mod.AddMember("file", rapidjson::Value{file->name, mBuildCache.allocator()}, mBuildCache.allocator());
        mod.AddMember("exported", token.exported, mBuildCache.allocator());
    }

    auto processModulePartition(const rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        rapidjson::Value &partitions = moduleValue(token.moduleName)["partitions"];
        partitions.AddMember(rapidjson::Value{token.name, mBuildCache.allocator()}, rapidjson::Value{file->name, mBuildCache.allocator()}, mBuildCache.allocator());
    }

    auto processToken(rapidjson::Value::MemberIterator &file, const Token &token) -> void
    {
        switch (token.type)
        {
        case Token::Type::IncludeLocal:
            processIncludeImport(value(file, "includes_local"), token);
            break;
        case Token::Type::IncludeExternal:
            processIncludeImport(value(file, "includes_external"), token);
            break;
        case Token::Type::Module:
            processModule(file, token);
            break;
        case Token::Type::ModulePartition:
            processModulePartition(file, token);
            break;
        case Token::Type::ImportModule:
            processIncludeImport(value(file, "import_modules"), token);
            break;
        case Token::Type::ImportIncludeLocal:
            processIncludeImport(value(file, "import_includes_local"), token);
            break;
        case Token::Type::ImportIncludeExternal:
            processIncludeImport(value(file, "import_includes_external"), token);
            break;
        case Token::Type::ImportModulePartition:
            processIncludeImport(value(file, "import_module_partitions"), token);
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
            file->value.AddMember(rapidjson::Value{valueName, mBuildCache.allocator()}, rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        }

        return file->value[valueName];
    }

    BuildCache &mBuildCache;
};
}
