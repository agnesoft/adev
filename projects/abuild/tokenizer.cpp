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
        ImportHeaderLocal,
        ImportHeaderExternal,
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

    [[nodiscard]] auto next() -> Token
    {
        while (!atEnd())
        {
            skipWhiteSpace();

            const char c = mContent[pos++];

            if (c == '#' && isInclude())
            {
                return extractInclude();
            }
            else if (c == 'i' && isImport())
            {
                return extractImport();
            }
            else if (c == 'm' && isModule())
            {
                return extractModule();
            }
            else if (c == 'e' && isExport())
            {
                return extractExport();
            }
            else if (c == '/')
            {
                skipComment();
            }
            else
            {
                skipToSemicolon();
            }
        }

        return {};
    }

private:
    [[nodiscard]] auto atEnd() const noexcept -> bool
    {
        return pos >= mContent.size();
    }

    [[nodiscard]] auto extractExport() -> Token
    {
        Token token;

        skipWhiteSpace();

        if (mContent[pos] == 'i')
        {
            pos += 6;
            skipWhiteSpace();
            token = extractImport();
        }
        else if (mContent[pos] == 'm')
        {
            pos += 6;
            skipWhiteSpace();
            token = extractModule();
        }
        else
        {
            token.name += mContent[pos];
        }

        token.exported = true;
        return token;
    }

    [[nodiscard]] auto extractImport() -> Token
    {
        Token token;
        skipWhiteSpace();

        if (mContent[pos] == ':')
        {
            token.type = Token::Type::ImportModulePartition;
            pos++;
            skipWhiteSpace();
        }
        else if (mContent[pos] == '"')
        {
            pos++;
            token.type = Token::Type::ImportHeaderLocal;
        }
        else if (mContent[pos] == '<')
        {
            pos++;
            token.type = Token::Type::ImportHeaderExternal;
        }
        else
        {
            token.type = Token::Type::ImportModule;
        }

        while (mContent[pos] != '"' && mContent[pos] != '>' && mContent[pos] != ';' && !atEnd())
        {
            token.name += mContent[pos++];
        }

        return token;
    }

    [[nodiscard]] auto extractInclude() -> Token
    {
        Token token;
        pos++;

        while (mContent[pos] != '"' && mContent[pos] != '>' && !atEnd())
        {
            token.name += mContent[pos++];
        }

        token.type = mContent[pos] == '"' ? Token::Type::IncludeLocal : Token::Type::IncludeExternal;
        return token;
    }

    [[nodiscard]] auto extractModule() -> Token
    {
        Token token;
        token.type = Token::Type::Module;

        while (mContent[pos] != ':' && mContent[pos] != ';' && !atEnd())
        {
            if (std::isspace(mContent[pos]))
            {
                pos++;
                continue;
            }

            token.name += mContent[pos++];
        }

        if (mContent[pos++] == ':')
        {
            token.moduleName = token.name;
            token.type = Token::Type::ModulePartition;
            token.name.clear();

            while (mContent[pos] != ';' && !atEnd())
            {
                if (std::isspace(mContent[pos]))
                {
                    pos++;
                    continue;
                }

                token.name += mContent[pos++];
            }
        }

        return token;
    }

    [[nodiscard]] auto isExport() -> bool
    {
        if (mContent.substr(pos, 5) == "xport")
        {
            pos += 5;

            if (mContent[pos] == ' ')
            {
                skipWhiteSpace();

                const std::string what = mContent.substr(pos, 6);
                return what == "import" || what == "module";
            }
        }

        return false;
    }

    [[nodiscard]] auto isImport() -> bool
    {
        if (mContent.substr(pos, 5) == "mport")
        {
            pos += 5;
            return mContent[pos] == ' ' || mContent[pos] == '\t' || mContent[pos] == '"' || mContent[pos] == '<' || mContent[pos] == ':';
        }

        return false;
    }

    [[nodiscard]] auto isInclude() -> bool
    {
        if (mContent.substr(pos, 7) == "include")
        {
            pos += 7;
            skipWhiteSpace();
            return mContent[pos] == '"' || mContent[pos] == '<';
        }

        return false;
    }

    [[nodiscard]] auto isModule() -> bool
    {
        if (mContent.substr(pos, 5) == "odule")
        {
            pos += 5;
            skipWhiteSpace();
            return mContent[pos] == ' ';
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
    }

    auto skipLine() -> void
    {
        while (mContent[pos] != '\n' && !atEnd())
        {
            pos++;
        }
    }

    auto skipToSemicolon() -> void
    {
        while (mContent[pos] != ';' && mContent[pos] != '\n' && !atEnd())
        {
            pos++;
        }
    }

    auto skipMultiLineComment() -> void
    {
        while (mContent[pos] != '*' && mContent[pos + 1] != '/' && !atEnd())
        {
            pos++;
        }
    }

    auto skipWhiteSpace() -> void
    {
        while (std::isspace(mContent[pos]))
        {
            pos++;
        }
    }

    size_t pos = 0;
    std::string mContent;
};
}
