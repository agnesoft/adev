#ifdef _MSC_VER
export module abuild : cpp_file;
import : file;
#endif

namespace abuild
{
export class CppFile : public File
{
public:
    using File::File;

    auto addImportExternal(const std::string &name, File *file) -> void
    {
        mImportsExternal.try_emplace(name, file);
    }

    auto addImportLocal(const std::string &name, File *file) -> void
    {
        mImportsLocal.try_emplace(name, file);
    }

    auto addImportModulePartition(const std::string &name, File *file) -> void
    {
        mImportModulePartitions.try_emplace(name, file);
    }

    auto addImportModule(const std::string &name, File *file) -> void
    {
        mImportModules.try_emplace(name, file);
    }

    auto addIncludeExternal(const std::string &name, File *file) -> void
    {
        mIncludesExternal.try_emplace(name, file);
    }

    auto addIncludeLocal(const std::string &name, File *file) -> void
    {
        mIncludesLocal.try_emplace(name, file);
    }

    [[nodiscard]] auto importsExternal() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mImportsExternal;
    }

    [[nodiscard]] auto importsLocal() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mImportsLocal;
    }

    [[nodiscard]] auto importModulePartitions() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mImportModulePartitions;
    }

    [[nodiscard]] auto importModules() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mImportModules;
    }

    [[nodiscard]] auto includesExternal() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mIncludesExternal;
    }

    [[nodiscard]] auto includesLocal() const noexcept -> const std::unordered_map<std::string, File *> &
    {
        return mIncludesLocal;
    }

    [[nodiscard]] auto isModule() const noexcept -> bool
    {
        return !mModule.empty() && mModulePartition.empty();
    }

    [[nodiscard]] auto isModulePartition() const noexcept -> bool
    {
        return !mModulePartition.empty();
    }

    [[nodiscard]] auto moduleName() const noexcept -> const std::string &
    {
        return mModule;
    }

    [[nodiscard]] auto modulePartition() const noexcept -> const std::string &
    {
        return mModulePartition;
    }

    auto setModuleName(std::string module) noexcept -> void
    {
        mModule = std::move(module);
    }

    auto setModulePartition(std::string module, std::string partition) -> void
    {
        mModule = std::move(module);
        mModulePartition = std::move(partition);
    }

private:
    std::string mModule;
    std::string mModulePartition;
    std::unordered_map<std::string, File *> mImportsExternal;
    std::unordered_map<std::string, File *> mImportsLocal;
    std::unordered_map<std::string, File *> mImportModulePartitions;
    std::unordered_map<std::string, File *> mImportModules;
    std::unordered_map<std::string, File *> mIncludesExternal;
    std::unordered_map<std::string, File *> mIncludesLocal;
};
}
