#ifndef __clang__
module abuild.cache:configuration_index;
import :translation_unit;
#endif

namespace abuild
{
class ConfigurationIndex
{
public:
    [[nodiscard]] auto header_unit(CppFile *file) const -> HeaderUnit *
    {
        auto it = this->headerUnits.find(file);

        if (it != this->headerUnits.end())
        {
            return it->second;
        }

        return nullptr;
    }

    auto insert(HeaderUnit *unit) -> void
    {
        this->headerUnits.insert({unit->file, unit});
    }

    auto insert(Module *mod) -> void
    {
        this->modules.insert({mod->name, mod});
    }

    auto insert(ModulePartition *partition) -> void
    {
        this->modulePartitions.insert({ConfigurationIndex::module_partition_name(partition->mod->name, partition->name), partition});
    }

    [[nodiscard]] auto module_(const std::string &name) const -> Module *
    {
        auto it = this->modules.find(name);

        if (it != this->modules.end())
        {
            return it->second;
        }

        return nullptr;
    }

    [[nodiscard]] auto module_partition(const std::string &mod, const std::string &partition) const -> ModulePartition *
    {
        auto it = this->modulePartitions.find(ConfigurationIndex::module_partition_name(mod, partition));

        if (it != this->modulePartitions.end())
        {
            return it->second;
        }

        return nullptr;
    }

private:
    [[nodiscard]] static auto module_partition_name(const std::string &mod, const std::string &partition) -> std::string
    {
        return mod + ':' + partition;
    }

    std::unordered_map<CppFile *, HeaderUnit *> headerUnits;
    std::unordered_map<std::string, Module *> modules;
    std::unordered_map<TranslationUnit *, Module *> moduleTranslationUnits;
    std::unordered_map<std::string, ModulePartition *> modulePartitions;
};
}
