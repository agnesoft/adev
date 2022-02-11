#ifndef __clang__
export module abuild.cpptokenizer : token_operators;
export import : token;
#endif

namespace abuild
{
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
    return stream << "__has_include(\"" << token.name << "\")";
}

export auto operator<<(std::ostream &stream, const HasIncludeExternalToken &token) -> std::ostream &
{
    return stream << "__has_include(<" << token.name << ">)";
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const LeftBracketToken &token) -> std::ostream &
{
    return stream << '(';
}

export auto operator<<(std::ostream &stream, [[maybe_unused]] const RightBracketToken &token) -> std::ostream &
{
    return stream << ')';
}

export auto operator<<(std::ostream &stream, const DefineToken &token) -> std::ostream &
{
    stream << "#define " << token.name;

    if (!token.value.empty())
    {
        stream << ' ' << token.value;
    }

    return stream;
}

export auto operator<<(std::ostream &stream, const ImportIncludeExternalToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export ";
    }

    return stream << "import <" << token.name << ">;";
}

export auto operator<<(std::ostream &stream, const ImportIncludeLocalToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export ";
    }

    return stream << "import \"" << token.name << "\";";
}

export auto operator<<(std::ostream &stream, const ImportModulePartitionToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export ";
    }

    return stream << "import : " << token.name << ';';
}

export auto operator<<(std::ostream &stream, const ImportModuleToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export ";
    }

    return stream << "import " << token.name << ';';
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
        stream << "export ";
    }

    return stream << "module " << token.mod << " : " << token.name << ';';
}

export auto operator<<(std::ostream &stream, const ModuleToken &token) -> std::ostream &
{
    if (token.exported)
    {
        stream << "export ";
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
