#ifdef _MSC_VER
export module abuild : tokenizer;
import "astl.hpp";
#endif

namespace abuild
{
export struct Token
{
    enum class Type
    {
        None = 0,
        IncludeLocal,
        IncludeExternal,
        Module,
        ModulePartition,
        ImportModule,
        ImportIncludeLocal,
        ImportIncludeExternal,
        ImportModulePartition
    };

    std::string name;
    std::string moduleName;
    Type type = Type::None;
    bool exported = false;
};

export class Tokenizer
{
public:
    explicit Tokenizer(const std::string &content) :
        mContent{content}
    {
    }

    [[nodiscard]] auto next() -> std::optional<Token>
    {
        while (!atEnd())
        {
            skipWhiteSpaceOrComment();

            const char c = mContent[pos++];

            if (c == '#')
            {
                if (isInclude())
                {
                    return extractInclude();
                }
                else
                {
                    skipLine();
                }
            }
            else if (c == 'i')
            {
                if (isImport())
                {
                    return extractImport();
                }
                else
                {
                    skipToSemicolonOrLine();
                }
            }
            else if (c == 'm')
            {
                if (isModule())
                {
                    return extractModule();
                }
                else
                {
                    skipToSemicolonOrLine();
                }
            }
            else if (c == 'e')
            {
                if (isExport())
                {
                    return extractExport();
                }
                else
                {
                    skipToSemicolonOrLine();
                }
            }
            else
            {
                skipToSemicolonOrLine();
            }
        }

        return std::nullopt;
    }

private:
    [[nodiscard]] auto atEnd() const noexcept -> bool
    {
        return pos >= mContent.size();
    }

    [[nodiscard]] auto extractExport() -> Token
    {
        skipWhiteSpaceOrComment();
        Token token = extractExportToken();
        token.exported = true;
        return token;
    }

    [[nodiscard]] auto extractExportToken() -> Token
    {
        if (mContent[pos] == 'i')
        {
            return extractExportImport();
        }
        else if (mContent[pos] == 'm')
        {
            return extractExportModule();
        }
        else
        {
            throw std::runtime_error{"Unknown export at '" + mContent.substr(pos, 10) + '\''};
        }
    }

    [[nodiscard]] auto extractExportImport() -> Token
    {
        pos += 6;
        skipWhiteSpaceOrComment();
        return extractImport();
    }

    [[nodiscard]] auto extractExportModule() -> Token
    {
        pos += 6;
        skipWhiteSpaceOrComment();
        return extractModule();
    }

    [[nodiscard]] auto extractImport() -> Token
    {
        skipWhiteSpaceOrComment();

        if (mContent[pos] == ':')
        {
            pos++;
            return extractImportToken(Token::Type::ImportModulePartition);
        }
        else if (mContent[pos] == '"')
        {
            pos++;
            return extractImportToken(Token::Type::ImportIncludeLocal);
        }
        else if (mContent[pos] == '<')
        {
            pos++;
            return extractImportToken(Token::Type::ImportIncludeExternal);
        }
        else
        {
            return extractImportToken(Token::Type::ImportModule);
        }
    }

    [[nodiscard]] auto extractImportToken(Token::Type type) -> Token
    {
        Token token;
        token.type = type;
        token.name = extractTokenName();
        return token;
    }

    [[nodiscard]] auto extractInclude() -> Token
    {
        Token token;
        token.type = mContent[pos] == '"' ? Token::Type::IncludeLocal : Token::Type::IncludeExternal;
        pos++;
        token.name = extractTokenName();
        return token;
    }

    [[nodiscard]] auto extractModule() -> Token
    {
        Token token;
        std::string tokenName = extractTokenName();

        if (mContent[pos++] == ':')
        {
            token.type = Token::Type::ModulePartition;
            token.name = extractTokenName();
            token.moduleName = std::move(tokenName);
        }
        else
        {
            token.type = Token::Type::Module;
            token.name = std::move(tokenName);
        }

        return token;
    }

    [[nodiscard]] auto extractTokenName() -> std::string
    {
        std::string name;

        while (mContent[pos] != '"' && mContent[pos] != '>' && mContent[pos] != ':' && mContent[pos] != ';' && !atEnd())
        {
            if (isComment())
            {
                skipMultiLineComment();
            }
            else
            {
                name += mContent[pos++];
            }
        }

        return trim(name);
    }

    [[nodiscard]] auto isComment() -> bool
    {
        return mContent[pos] == '/' && mContent[pos + 1] == '*';
    }

    [[nodiscard]] auto isExport() -> bool
    {
        const std::string EXPORT = "xport";

        if (mContent.substr(pos, EXPORT.size()) == EXPORT)
        {
            pos += EXPORT.size();

            const bool result = std::isspace(mContent[pos]) || isComment();
            skipWhiteSpaceOrComment();
            const std::string what = mContent.substr(pos, 6);
            return result && (what == "import" || what == "module");
        }

        return false;
    }

    [[nodiscard]] auto isImport() -> bool
    {
        const std::string IMPORT = "mport";

        if (mContent.substr(pos, IMPORT.size()) == IMPORT)
        {
            pos += IMPORT.size();
            return std::isspace(mContent[pos]) || mContent[pos] == '"' || mContent[pos] == '<' || mContent[pos] == ':' || isComment();
        }

        return false;
    }

    [[nodiscard]] auto isInclude() -> bool
    {
        const std::string INCLUDE = "include";
        skipWhiteSpaceOrComment();

        if (mContent.substr(pos, INCLUDE.size()) == INCLUDE)
        {
            pos += INCLUDE.size();
            skipWhiteSpaceOrComment();
            return mContent[pos] == '"' || mContent[pos] == '<';
        }

        return false;
    }

    [[nodiscard]] auto isModule() -> bool
    {
        const std::string MODULE = "odule";

        if (mContent.substr(pos, MODULE.size()) == MODULE)
        {
            pos += MODULE.size();
            const bool result = std::isspace(mContent[pos]) || isComment();
            skipWhiteSpaceOrComment();
            return result;
        }

        return false;
    }

    auto skipComment() -> void
    {
        if (mContent[pos] == '/')
        {
            skipLine();
        }
        else if (mContent[pos] == '*')
        {
            skipMultiLineComment();
        }
        else
        {
            pos++;
        }
    }

    auto skipLine() -> void
    {
        while (mContent[pos] != '\n' && !atEnd())
        {
            pos++;
        }

        pos++;
    }

    auto skipToSemicolonOrLine() -> void
    {
        while (mContent[pos] != ';' && mContent[pos] != '\n' && !atEnd())
        {
            if (mContent[pos] == '/')
            {
                skipComment();
            }
            else
            {
                pos++;
            }
        }

        pos++;
    }

    auto skipMultiLineComment() -> void
    {
        while (!(mContent[pos] == '/' && mContent[pos - 1] == '*') && !atEnd())
        {
            pos++;
        }

        pos++;
    }

    auto skipWhiteSpaceOrComment() -> void
    {
        while (!atEnd())
        {
            const char c = mContent[pos];

            if (c == '/')
            {
                pos++;
                skipComment();
            }
            else if (!std::isspace(c))
            {
                break;
            }
            else
            {
                pos++;
            }
        }
    }

    [[nodiscard]] static auto trim(const std::string &str) -> std::string
    {
        size_t prefix = 0;
        size_t suffix = str.size();

        while (std::isspace(str[prefix]) && prefix < str.size())
        {
            prefix++;
        }

        while (std::isspace(str[suffix - 1]) && 0 < suffix)
        {
            suffix--;
        }

        return str.substr(prefix, suffix - prefix);
    }

    size_t pos = 0;
    std::string mContent;
};
}
