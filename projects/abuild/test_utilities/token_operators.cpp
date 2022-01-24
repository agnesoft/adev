#ifndef __clang__
export module abuild.test_utilities : token_operators;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const AndToken &left, [[maybe_unused]] const AndToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==(const DefinedToken &left, const DefinedToken &right) noexcept -> bool
{
    return left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const EqualsToken &left, [[maybe_unused]] const EqualsToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==(const GreaterThanOrEqualsToken &left, const GreaterThanOrEqualsToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

export constexpr [[nodiscard]] auto operator==(const GreaterThanToken &left, const GreaterThanToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const LeftBracketToken &left, [[maybe_unused]] const LeftBracketToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==(const LessThanOrEqualsToken &left, const LessThanOrEqualsToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

export constexpr [[nodiscard]] auto operator==(const LessThanToken &left, const LessThanToken &right) noexcept -> bool
{
    return left.left == right.left && left.right == right.right;
}

export constexpr [[nodiscard]] auto operator==(const HasIncludeLocalToken &left, const HasIncludeLocalToken &right) noexcept -> bool
{
    return left.include == right.include;
}

export constexpr [[nodiscard]] auto operator==(const HasIncludeExternalToken &left, const HasIncludeExternalToken &right) noexcept -> bool
{
    return left.include == right.include;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const NotToken &left, [[maybe_unused]] const NotToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const OrToken &left, [[maybe_unused]] const OrToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const RightBracketToken &left, [[maybe_unused]] const RightBracketToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==(const IfToken &left, const IfToken &right) noexcept -> bool
{
    return left.elements == right.elements;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const ElseToken &left, [[maybe_unused]] const ElseToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==([[maybe_unused]] const EndIfToken &left, [[maybe_unused]] const EndIfToken &right) noexcept -> bool
{
    return true;
}

export constexpr [[nodiscard]] auto operator==(const DefineToken &left, const DefineToken &right) noexcept -> bool
{
    return left.name == right.name && left.value == right.value;
}

export constexpr [[nodiscard]] auto operator==(const ImportIncludeExternalToken &left, const ImportIncludeExternalToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const ImportIncludeLocalToken &left, const ImportIncludeLocalToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const ImportModulePartitionToken &left, const ImportModulePartitionToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const ImportModuleToken &left, const ImportModuleToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const IncludeExternalToken &left, const IncludeExternalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const IncludeLocalToken &left, const IncludeLocalToken &right) noexcept -> bool
{
    return left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const ModulePartitionToken &left, const ModulePartitionToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.mod == right.mod && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const ModuleToken &left, const ModuleToken &right) noexcept -> bool
{
    return left.exported == right.exported && left.name == right.name;
}

export constexpr [[nodiscard]] auto operator==(const UndefToken &left, const UndefToken &right) noexcept -> bool
{
    return left.name == right.name;
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const AndToken &token) -> std::ostream &
{
    return stream << "&&";
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const OrToken &token) -> std::ostream &
{
    return stream << "||";
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const NotToken &token) -> std::ostream &
{
    return stream << '!';
}

export auto operator<<(std::ostream &stream, const DefinedToken &token) -> std::ostream &
{
    return stream << "defined(" << token.name << ')';
}

export auto operator<<(std::ostream &stream, const EqualsToken &token) -> std::ostream &
{
    return stream << token.left << " == " << token.right;
}

export auto operator<<(std::ostream &stream, const GreaterThanOrEqualsToken &token) -> std::ostream &
{
    return stream << token.left << " >= " << token.right;
}

export auto operator<<(std::ostream &stream, const GreaterThanToken &token) -> std::ostream &
{
    return stream << token.left << " > " << token.right;
}

export auto operator<<(std::ostream &stream, const LessThanOrEqualsToken &token) -> std::ostream &
{
    return stream << token.left << " <= " << token.right;
}

export auto operator<<(std::ostream &stream, const LessThanToken &token) -> std::ostream &
{
    return stream << token.left << " < " << token.right;
}

export auto operator<<(std::ostream &stream, const HasIncludeLocalToken &token) -> std::ostream &
{
    return stream << "__has_include(\"" << token.include << "\")";
}

export auto operator<<(std::ostream &stream, const HasIncludeExternalToken &token) -> std::ostream &
{
    return stream << "__has_include(<" << token.include << ">)";
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const LeftBracketToken &token) -> std::ostream &
{
    return stream << '(';
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const RightBracketToken &token) -> std::ostream &
{
    return stream << '(';
}

export auto operator<<(std::ostream &stream, const DefineToken &token) -> std::ostream &
{
    return stream << "#define " << token.name << ' ' << token.value;
}

export auto operator<<(std::ostream &stream, const ImportIncludeExternalToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import <";
    }

    return stream << token.name << ">;";
}

export auto operator<<(std::ostream &stream, const ImportIncludeLocalToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import \"";
    }

    return stream << token.name << "\";";
}

export auto operator<<(std::ostream &stream, const ImportModulePartitionToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import : ";
    }

    return stream << token.name << ';';
}

export auto operator<<(std::ostream &stream, const ImportModuleToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import ";
    }

    return stream << token.name << ';';
}

export auto operator<<(std::ostream &stream, const IncludeExternalToken &token) -> std::ostream &
{
    return stream << "#include <" << token.name << '>';
}

export auto operator<<(std::ostream &stream, const IncludeLocalToken &token) -> std::ostream &
{
    return stream << "#include \"" << token.name << '"';
}

export auto operator<<(std::ostream &stream, const ModulePartitionToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import ";
    }

    return stream << "module " << token.mod << " : " << token.name << ';';
}

export auto operator<<(std::ostream &stream, const ModuleToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export import ";
    }

    return stream << "module " << token.name << ';';
}

export auto operator<<(std::ostream &stream, const UndefToken &token) -> std::ostream &
{
    return stream << "#undef " << token.name;
}

export auto operator<<(std::ostream &stream, const IfToken &token) -> std::ostream &
{
    stream << "#if ";

    for (const IfElement &element : token.elements)
    {
        IfElement e = element;

        std::visit([&](auto &&value) {
            stream << value << ' ';
        },
                   e);
    }

    return stream;
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const ElseToken &token) -> std::ostream &
{
    return stream << "#else";
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const EndIfToken &token) -> std::ostream &
{
    return stream << "#endif";
}

export auto operator<<(std::ostream &stream, const Token &token) -> std::ostream &
{
    Token t = token;

    std::visit([&](auto &&value) {
        stream << value;
    },
               t);

    return stream;
}
}
