#ifndef __clang__
module abuild.cache : token_writer;
import abuild.cpptokenizer;
#endif

namespace abuild
{
class TokenWriter
{
    auto write_bool(bool value) -> void
    {
        this->stream << ' ' << value;
    }

    auto write_string(const std::string &value) -> void
    {
        this->stream << value.size() << ' ' << value;
    }

    auto write_variant(auto &variant) -> void
    {
        this->stream << variant.index() << ' ';

        std::visit([&](auto &&value) {
            this->write(value);
        },
                   variant);
    }

public:
    auto write(Token &token)
    {
        this->write_variant(token);
    }

    [[nodiscard]] auto to_string() const -> std::string
    {
        return this->stream.str();
    }

private:
    auto write([[maybe_unused]] AndToken &token) -> void
    {
    }

    auto write([[maybe_unused]] OrToken &token) -> void
    {
    }

    auto write([[maybe_unused]] NotToken &token) -> void
    {
    }

    auto write([[maybe_unused]] LeftBracketToken &token) -> void
    {
    }

    auto write([[maybe_unused]] RightBracketToken &token) -> void
    {
    }

    auto write([[maybe_unused]] ElseToken &token) -> void
    {
    }

    auto write([[maybe_unused]] EndIfToken &token) -> void
    {
    }

    auto write(DefinedToken &token) -> void
    {
        this->write_string(token.name);
    }

    auto write(EqualsToken &token) -> void
    {
        this->write_string(token.left);
        this->write_string(token.right);
    }

    auto write(GreaterThanOrEqualsToken &token) -> void
    {
        this->write_string(token.left);
        this->write_string(token.right);
    }

    auto write(GreaterThanToken &token) -> void
    {
        this->write_string(token.left);
        this->write_string(token.right);
    }

    auto write(LessThanOrEqualsToken &token) -> void
    {
        this->write_string(token.left);
        this->write_string(token.right);
    }

    auto write(LessThanToken &token) -> void
    {
        this->write_string(token.left);
        this->write_string(token.right);
    }

    auto write(HasIncludeLocalToken &token) -> void
    {
        this->write_string(token.name);
    }

    auto write(HasIncludeExternalToken &token) -> void
    {
        this->write_string(token.name);
    }

    auto write(IfToken &token) -> void
    {
        this->stream << token.elements.size();

        for (IfElement &e : token.elements)
        {
            this->stream << ' ';
            this->write_variant(e);
        }
    }

    auto write(DefineToken &token) -> void
    {
        this->write_string(token.name);
        this->write_string(token.value);
    }

    auto write(ImportIncludeExternalToken &token) -> void
    {
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(ImportIncludeLocalToken &token) -> void
    {
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(ImportModulePartitionToken &token) -> void
    {
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(ImportModuleToken &token) -> void
    {
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(IncludeExternalToken &token) -> void
    {
        this->write_string(token.name);
    }

    auto write(IncludeLocalToken &token) -> void
    {
        this->write_string(token.name);
    }

    auto write(ModulePartitionToken &token) -> void
    {
        this->write_string(token.mod);
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(ModuleToken &token) -> void
    {
        this->write_string(token.name);
        this->write_bool(token.exported);
    }

    auto write(UndefToken &token) -> void
    {
        this->write_string(token.name);
    }

    std::ostringstream stream;
};
}
