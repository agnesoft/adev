#ifdef _MSC_VER
export module abuild : tokenizer;
import<astl.hpp>;
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

struct BadToken
{
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
            try
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
            catch ([[maybe_unused]] BadToken &badToken)
            {
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
            throw BadToken{};
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
            skipWhiteSpaceOrComment();
            return extractImportToken(Token::Type::ImportModulePartition, ';');
        }
        else if (mContent[pos] == '"')
        {
            pos++;
            return extractImportToken(Token::Type::ImportIncludeLocal, '"');
        }
        else if (mContent[pos] == '<')
        {
            pos++;
            return extractImportToken(Token::Type::ImportIncludeExternal, '>');
        }
        else
        {
            return extractImportToken(Token::Type::ImportModule, ';');
        }
    }

    [[nodiscard]] auto extractImportToken(Token::Type type, const char separator) -> Token
    {
        Token token;
        token.type = type;
        token.name = separator == '"' || separator == '>' ? extractIncludeName(separator) : extractTokenName();
        pos++;
        return token;
    }

    [[nodiscard]] auto extractInclude() -> Token
    {
        Token token;
        const char separator = mContent[pos++];
        token.type = separator == '"' ? Token::Type::IncludeLocal : Token::Type::IncludeExternal;
        token.name = extractIncludeName(separator == '"' ? '"' : '>');
        return token;
    }

    [[nodiscard]] auto extractModule() -> Token
    {
        Token token;
        std::string tokenName = extractTokenName();

        if (mContent[pos++] == ':')
        {
            token.type = Token::Type::ModulePartition;
            skipWhiteSpaceOrComment();
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

    [[nodiscard]] auto extractIncludeName(const char separator) -> std::string
    {
        std::string name;

        while (mContent[pos] != separator && mContent[pos] != '\n' && !atEnd())
        {
            name += mContent[pos++];
        }

        if (mContent[pos] != separator)
        {
            throw BadToken{};
        }

        return name;
    }

    [[nodiscard]] auto extractTokenName() -> std::string
    {
        std::string name;

        while (mContent[pos] != ':' && mContent[pos] != ';' && !std::isspace(mContent[pos]) && !atEnd())
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

        skipWhiteSpaceOrComment();

        if (mContent[pos] != ':' && mContent[pos] != ';')
        {
            throw BadToken{};
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

    [[nodiscard]] auto matchSequence(const std::string &sequence) -> bool
    {
        for (const char c : sequence)
        {
            if (mContent[pos++] != c || atEnd())
            {
                return false;
            }
        }

        return true;
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

    auto skipRawString() -> void
    {
        const std::string sequence = skipSequence();

        while (!matchSequence(sequence) && !atEnd())
        {
        }
    }

    [[nodiscard]] auto skipSequence() -> std::string
    {
        std::string sequence{')'};

        while (mContent[pos] != '(' && !atEnd())
        {
            sequence += mContent[pos++];
        }

        pos++;
        sequence += '"';

        return sequence;
    }

    auto skipStringLiteral() -> void
    {
        pos++;

        if (mContent[pos - 2] == 'R')
        {
            skipRawString();
        }
        else
        {
            skipString();
        }
    }

    auto skipString() -> void
    {
        while (mContent[pos++] != '"' && mContent[pos - 2] != '\\' && !atEnd())
        {
        }
    }

    auto skipToSemicolonOrLine() -> void
    {
        while (mContent[pos] != ';' && mContent[pos] != '\n' && !atEnd())
        {
            if (mContent[pos] == '/')
            {
                skipComment();
            }
            else if (mContent[pos] == '"')
            {
                skipStringLiteral();
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
