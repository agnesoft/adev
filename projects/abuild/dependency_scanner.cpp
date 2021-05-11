#ifdef _MSC_VER
export module abuild : dependency_scanner;
import : build_cache;
#endif

namespace abuild
{
export class DependencyScanner
{
public:
    explicit DependencyScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        processFiles(mBuildCache.sources());
        processFiles(mBuildCache.headers());
    }

private:
    [[nodiscard]] auto addHeader(rapidjson::Value::MemberIterator include) -> bool
    {
        rapidjson::Value::MemberIterator includeFile = findHeader(include->name.GetString());

        if (includeFile != rapidjson::Value::MemberIterator{})
        {
            include->value = rapidjson::Value{includeFile->name, mBuildCache.allocator()};
            return true;
        }

        return false;
    }

    auto addSource(rapidjson::Value::MemberIterator include)
    {
        rapidjson::Value::MemberIterator includeFile = findSource(include->name.GetString());

        if (includeFile != rapidjson::Value::MemberIterator{})
        {
            include->value = rapidjson::Value{includeFile->name, mBuildCache.allocator()};
        }
    }

    [[nodiscard]] auto findHeader(const std::string &include) -> rapidjson::Value::MemberIterator
    {
        for (rapidjson::Value::MemberIterator header = mBuildCache.headers().MemberBegin(); header != mBuildCache.headers().MemberEnd(); ++header)
        {
            if (isInclude(include, header->name.GetString()))
            {
                return header;
            }
        }

        return {};
    }

    [[nodiscard]] auto findSource(const std::string &include) -> rapidjson::Value::MemberIterator
    {
        for (rapidjson::Value::MemberIterator source = mBuildCache.sources().MemberBegin(); source != mBuildCache.sources().MemberEnd(); ++source)
        {
            if (isInclude(include, source->name.GetString()))
            {
                return source;
            }
        }

        return {};
    }

    [[nodiscard]] static auto isInclude(const std::string &include, const std::string &file) -> bool
    {
        return file.ends_with(std::filesystem::path{include}.make_preferred().string());
    }

    [[nodiscard]] auto isSTL(const std::string &include) -> bool
    {
        return CPP_STL.contains(include);
    }

    auto processExternalInclude(rapidjson::Value::MemberIterator include) -> void
    {
        if (isSTL(include->name.GetString()))
        {
            include->value = "STL";
        }
        else
        {
            if (!addHeader(include))
            {
                addSource(include);
            }
        }
    }

    auto processLocalInclude(rapidjson::Value::MemberIterator include, const std::filesystem::path &currentPath) -> void
    {
        rapidjson::Value::MemberIterator includeFile = findHeader((currentPath / include->name.GetString()).string());

        if (includeFile == rapidjson::Value::MemberIterator{})
        {
            includeFile = findSource((currentPath / include->name.GetString()).string());
        }

        if (includeFile == rapidjson::Value::MemberIterator{})
        {
            processExternalInclude(include);
        }
        else
        {
            include->value = rapidjson::Value{includeFile->name, mBuildCache.allocator()};
        }
    }

    auto processModule(rapidjson::Value::MemberIterator mod) -> void
    {
        if (mBuildCache.modules().HasMember(mod->name))
        {
            mod->value = rapidjson::Value{mBuildCache.modules()[mod->name]["file"], mBuildCache.allocator()};
        }
    }

    auto processModulePartition(rapidjson::Value::MemberIterator partition, const std::string &mod) -> void
    {
        if (mBuildCache.modules().HasMember(mod) && mBuildCache.modules()[mod]["partitions"].HasMember(partition->name))
        {
            partition->value = rapidjson::Value{mBuildCache.modules()[mod]["partitions"][partition->name], mBuildCache.allocator()};
        }
    }

    auto processFile([[maybe_unused]] rapidjson::Value::MemberIterator &file) -> void
    {
        scanIncludesExternal(file);
        scanIncludesLocal(file);
        scanImportModules(file);
        scanImportIncludesLocal(file);
        scanImportIncludesExternal(file);
        scanImportPartitions(file);
    }

    auto processFiles(rapidjson::Value &files) -> void
    {
        for (rapidjson::Value::MemberIterator file = files.MemberBegin(); file != files.MemberEnd(); ++file)
        {
            processFile(file);
        }
    }

    auto scanIncludesExternal(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("includes_external"))
        {
            for (rapidjson::Value::MemberIterator include = file->value["includes_external"].MemberBegin(); include != file->value["includes_external"].MemberEnd(); ++include)
            {
                processExternalInclude(include);
            }
        }
    }

    auto scanIncludesLocal(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("includes_local"))
        {
            for (rapidjson::Value::MemberIterator include = file->value["includes_local"].MemberBegin(); include != file->value["includes_local"].MemberEnd(); ++include)
            {
                processLocalInclude(include, std::filesystem::path{file->name.GetString()}.parent_path());
            }
        }
    }

    auto scanImportModules(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("import_modules"))
        {
            for (rapidjson::Value::MemberIterator mod = file->value["import_modules"].MemberBegin(); mod != file->value["import_modules"].MemberEnd(); ++mod)
            {
                processModule(mod);
            }
        }
    }

    auto scanImportIncludesLocal(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("import_includes_local"))
        {
            for (rapidjson::Value::MemberIterator include = file->value["import_includes_local"].MemberBegin(); include != file->value["import_includes_local"].MemberEnd(); ++include)
            {
                processLocalInclude(include, std::filesystem::path{file->name.GetString()}.parent_path());
            }
        }
    }

    auto scanImportIncludesExternal(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("import_includes_external"))
        {
            for (rapidjson::Value::MemberIterator include = file->value["import_includes_external"].MemberBegin(); include != file->value["import_includes_external"].MemberEnd(); ++include)
            {
                processExternalInclude(include);
            }
        }
    }

    auto scanImportPartitions(rapidjson::Value::MemberIterator file) -> void
    {
        if (file->value.HasMember("import_module_partitions"))
        {
            for (rapidjson::Value::MemberIterator partition = file->value["import_module_partitions"].MemberBegin(); partition != file->value["import_module_partitions"].MemberEnd(); ++partition)
            {
                if (file->value.HasMember("module"))
                {
                    processModulePartition(partition, file->value["module"].MemberBegin()->name.GetString());
                }
            }
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
        "coroutines",
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
