#ifdef _MSC_VER
export module abuild : code_scanner;
export import : tokenizer;
import : build_cache;
import : settings;
#endif

namespace abuild
{
export class CodeScanner
{
public:
    explicit CodeScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        scanSources();
        scanHeaders();
    }

private:
    [[nodiscard]] static auto isSource(const std::string token) -> bool
    {
        return Settings::cppSourceExtensions().contains(std::filesystem::path{token}.extension().string());
    }

    [[nodiscard]] auto isSTLHeader(const std::string token) -> bool
    {
        return CPP_STL.contains(token);
    }

    auto processImportIncludeExternalToken(const ImportIncludeExternalToken *value, File *file) -> void
    {
        if (isSource(value->name))
        {
            throw 4;
        }
        else if (isSTLHeader(value->name))
        {
            file->addDependency(ImportSTLHeaderDependency{.name = value->name});
        }
        else
        {
            file->addDependency(ImportExternalHeaderDependency{.name = value->name});
        }
    }

    auto processImportIncludeLocalToken(const ImportIncludeLocalToken *value, File *file) -> void
    {
        if (isSource(value->name))
        {
            throw 3;
        }
        else if (isSTLHeader(value->name))
        {
            file->addDependency(ImportSTLHeaderDependency{.name = value->name});
        }
        else
        {
            file->addDependency(ImportLocalHeaderDependency{.name = value->name});
        }
    }

    auto processIncludeExternalToken(const IncludeExternalToken *value, File *file) -> void
    {
        if (isSource(value->name))
        {
            throw 2;
        }
        else if (isSTLHeader(value->name))
        {
            file->addDependency(IncludeSTLHeaderDependency{.name = value->name});
        }
        else
        {
            file->addDependency(IncludeExternalHeaderDependency{.name = value->name});
        }
    }

    auto processIncludeLocalToken(const IncludeLocalToken *value, File *file) -> void
    {
        if (isSource(value->name))
        {
            file->addDependency(IncludeLocalSourceDependency{.name = value->name});
        }
        else if (isSTLHeader(value->name))
        {
            file->addDependency(IncludeSTLHeaderDependency{.name = value->name});
        }
        else
        {
            file->addDependency(IncludeLocalHeaderDependency{.name = value->name});
        }
    }

    auto processFile(const Token &token, File *file) -> void
    {
        if (auto *value = std::get_if<IncludeExternalToken>(&token))
        {
            processIncludeExternalToken(value, file);
            return;
        }

        if (auto *value = std::get_if<IncludeLocalToken>(&token))
        {
            processIncludeLocalToken(value, file);
            return;
        }

        if (auto *value = std::get_if<ImportModuleToken>(&token))
        {
            file->addDependency(ImportModuleDependency{.name = value->name});
            return;
        }

        if (auto *value = std::get_if<ImportIncludeLocalToken>(&token))
        {
            processImportIncludeLocalToken(value, file);
            return;
        }

        if (auto *value = std::get_if<ImportIncludeExternalToken>(&token))
        {
            processImportIncludeExternalToken(value, file);
            return;
        }

        throw 0;
    }

    auto processHeader(const Token &token, Header *file) -> void
    {
        if (std::get_if<ModuleToken>(&token)
            || std::get_if<ModulePartitionToken>(&token)
            || std::get_if<ImportModulePartitionToken>(&token))
        {
            throw 5;
        }
        else
        {
            processFile(token, file);
        }
    }

    auto processSource(const Token &token, Source *file) -> void
    {
        if (auto *value = std::get_if<ModuleToken>(&token))
        {
            mBuildCache.addModuleInterface(value->name, file);
            return;
        }

        if (auto *value = std::get_if<ModulePartitionToken>(&token))
        {
            mBuildCache.addModulePartition(value->mod, value->name, file);
            return;
        }

        if (auto *value = std::get_if<ImportModulePartitionToken>(&token))
        {
            file->addDependency(ImportModulePartitionDependency{.name = value->name});
            return;
        }

        processFile(token, file);
    }

    auto scanHeader(Header *header) -> void
    {
        Tokenizer tokenizer{header->content()};

        for (Token token = tokenizer.next(); std::holds_alternative<std::monostate>(token); token = tokenizer.next())
        {
            processHeader(token, header);
        }
    }

    auto scanSource(Source *source) -> void
    {
        Tokenizer tokenizer{source->content()};

        for (Token token = tokenizer.next(); std::holds_alternative<std::monostate>(token); token = tokenizer.next())
        {
            processSource(token, source);
        }
    }

    auto scanHeaders() -> void
    {
        for (const std::unique_ptr<Header> &header : mBuildCache.headers())
        {
            scanHeader(header.get());
        }
    }

    auto scanSources() -> void
    {
        for (const std::unique_ptr<Source> &source : mBuildCache.sources())
        {
            scanSource(source.get());
        }
    }

    BuildCache &mBuildCache;

    const std::unordered_set<std::string> CPP_STL = {
        "algorithm",
        "any",
        "array",
        "atomic",
        "barrier",
        "bit",
        "bitset",
        "cassert",
        "cctype",
        "cerrno",
        "cfenv",
        "cfloat",
        "charconv",
        "chrono",
        "cinttypes",
        "climits",
        "clocale",
        "cmath",
        "compare",
        "complex",
        "concepts",
        "condition_variable",
        "coroutine",
        "csetjmp",
        "csignal",
        "cstdarg",
        "cstddef",
        "cstdint",
        "cstdio",
        "cstdlib",
        "cstring",
        "ctime",
        "cuchar",
        "cwchar",
        "cwctype",
        "deque",
        "exception",
        "execution",
        "filesystem",
        "format",
        "forward_list",
        "fstream",
        "functional",
        "future",
        "initializer_list",
        "iomanip",
        "ios",
        "iosfwd",
        "iostream",
        "istream",
        "iterator",
        "latch",
        "limits",
        "list",
        "locale",
        "map",
        "memory",
        "memory_resource",
        "mutex",
        "new",
        "numbers",
        "numeric",
        "optional",
        "ostream",
        "queue",
        "random",
        "ranges",
        "ratio",
        "regex",
        "scoped_allocator",
        "semaphore",
        "set",
        "shared_mutex",
        "source_location",
        "span:",
        "sstream",
        "stack",
        "stdexcept",
        "stop_token",
        "streambuf",
        "string",
        "string_view",
        "syncstream",
        "system_error",
        "thread",
        "tuple",
        "type_traits",
        "typeindex",
        "typeinfo",
        "unordered_map",
        "unordered_set",
        "utility",
        "valarray",
        "variant",
        "vector",
        "version"};
};
}
