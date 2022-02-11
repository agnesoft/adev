#ifndef __clang__
module abuild.cache : token_reader;
import abuild.cpptokenizer;
#endif

namespace abuild
{
class TokenReader
{
    template<typename... Ts>
    static auto assign_variant(std::variant<Ts...> &var, std::size_t i) -> void
    {
        if (i < std::variant_size_v<std::variant<Ts...>>)
        {
            static constexpr auto table = std::array{+[]() { return std::variant<Ts...>{Ts{}}; }...};
            var = table[i]();
        }
    }

    [[nodiscard]] auto read_bool() -> bool
    {
        bool value = false;
        this->stream >> value;
        return value;
    }

    [[nodiscard]] auto read_string() -> std::string
    {
        std::string str;
        std::size_t size = 0;
        this->stream >> size;
        str.resize(size);
        this->stream.ignore();
        this->stream.read(str.data(), size);
        return str;
    }

    auto read_variant(auto &variant) -> void
    {
        std::size_t index = std::variant_npos;
        this->stream >> index;
        TokenReader::assign_variant(variant, index);

        std::visit([&](auto &&value) {
            this->read(value);
        },
                   variant);
    }

public:
    explicit TokenReader(const std::string &data) :
        stream{data}
    {
    }

    [[nodiscard]] auto read() -> Token
    {
        Token token;
        this->read_variant(token);
        return token;
    }

private:
    auto read([[maybe_unused]] AndToken &token) -> void
    {
    }

    auto read([[maybe_unused]] OrToken &token) -> void
    {
    }

    auto read([[maybe_unused]] NotToken &token) -> void
    {
    }

    auto read([[maybe_unused]] LeftBracketToken &token) -> void
    {
    }

    auto read([[maybe_unused]] RightBracketToken &token) -> void
    {
    }

    auto read([[maybe_unused]] ElseToken &token) -> void
    {
    }

    auto read([[maybe_unused]] EndIfToken &token) -> void
    {
    }

    auto read(DefinedToken &token) -> void
    {
        token.name = this->read_string();
    }

    auto read(EqualsToken &token) -> void
    {
        token.left = this->read_string();
        token.right = this->read_string();
    }

    auto read(GreaterThanOrEqualsToken &token) -> void
    {
        token.left = this->read_string();
        token.right = this->read_string();
    }

    auto read(GreaterThanToken &token) -> void
    {
        token.left = this->read_string();
        token.right = this->read_string();
    }

    auto read(LessThanOrEqualsToken &token) -> void
    {
        token.left = this->read_string();
        token.right = this->read_string();
    }

    auto read(LessThanToken &token) -> void
    {
        token.left = this->read_string();
        token.right = this->read_string();
    }

    auto read(HasIncludeLocalToken &token) -> void
    {
        token.name = this->read_string();
    }

    auto read(HasIncludeExternalToken &token) -> void
    {
        token.name = this->read_string();
    }

    auto read(IfToken &token) -> void
    {
        std::size_t size = 0;
        this->stream >> size;
        token.elements.reserve(size);

        for (std::size_t i = 0; i != size; ++i)
        {
            this->read_variant(token.elements.emplace_back(IfElement{}));
        }
    }

    auto read(DefineToken &token) -> void
    {
        token.name = this->read_string();
        token.value = this->read_string();
    }

    auto read(ImportIncludeExternalToken &token) -> void
    {
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(ImportIncludeLocalToken &token) -> void
    {
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(ImportModulePartitionToken &token) -> void
    {
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(ImportModuleToken &token) -> void
    {
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(IncludeExternalToken &token) -> void
    {
        token.name = this->read_string();
    }

    auto read(IncludeLocalToken &token) -> void
    {
        token.name = this->read_string();
    }

    auto read(ModulePartitionToken &token) -> void
    {
        token.mod = this->read_string();
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(ModuleToken &token) -> void
    {
        token.name = this->read_string();
        token.exported = this->read_bool();
    }

    auto read(UndefToken &token) -> void
    {
        token.name = this->read_string();
    }

    std::istringstream stream;
};
}
