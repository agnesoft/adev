#ifndef __clang__
module abuild.cache:configuration_index;
import :translation_unit;
#endif

namespace abuild
{
//! \private
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

    [[nodiscard]] auto module_(const std::string &name) const -> Module * // NOLINT(readability-identifier-naming)
    {
        auto it = this->modules.find(name);

        if (it != this->modules.end())
        {
            return it->second;
        }

        return nullptr;
    }

private:
    std::unordered_map<CppFile *, HeaderUnit *> headerUnits;
    std::unordered_map<std::string, Module *> modules;
};
}
