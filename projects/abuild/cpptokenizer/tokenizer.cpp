#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : preprocessor_tokenizer;
#endif

namespace abuild
{
//! \private
class Tokenizer : public TokenizerCommon
{
public:
    explicit Tokenizer(std::string_view source) :
        TokenizerCommon{source, this->pos, this->tokens}
    {
    }

    [[nodiscard]] auto tokenize() -> std::vector<Token>
    {
        while (!this->at_end())
        {
            this->skip_space_comment_newline();

            if (this->current_char() == '#')
            {
                PreprocessorTokenizer{this->source_view(), this->pos_ref(), this->tokens_ref()}.tokenize();
            }
            else if (this->current_char() == '"')
            {
                this->skip_string();
            }
            else if (this->current_char() == 'e')
            {
                this->extract_e();
            }
            else if (this->current_char() == 'i')
            {
                this->extract_i(false);
            }
            else if (this->current_char() == 'm')
            {
                this->extract_m(false);
            }
            else if (!this->at_end())
            {
                this->skip_semicolon_newline();
            }
        }

        return tokens;
    }

private:
    template<typename T>
    auto import_include(std::string_view name, bool exported) -> void
    {
        this->skip_space_comment_newline();

        if (this->current_char() == ';' && !name.empty())
        {
            this->skip_one();
            this->push_token(T{
                .name = std::string(&name[0], name.size()),
                .exported = exported});
        }
        else
        {
            this->skip_space_comment();
        }
    }

    auto extract_e() -> void
    {
        std::string_view word = this->identifier();

        if (word == "export")
        {
            this->extract_export();
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    auto extract_i(bool exported) -> void
    {
        std::string_view word = this->identifier();

        if (word == "import")
        {
            this->extract_import(exported);
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    auto extract_import(bool exported) -> void
    {
        this->skip_space_comment_newline();

        if (this->current_char() == '"')
        {
            this->import_include<ImportIncludeLocalToken>(this->local_include(), exported);
        }
        else if (this->current_char() == '<')
        {
            this->import_include<ImportIncludeExternalToken>(this->external_include(), exported);
        }
    }

    auto extract_export() -> void
    {
        this->skip_space_comment_newline();

        if (this->current_char() == 'i')
        {
            this->extract_i(true);
        }
        else if (this->current_char() == 'm')
        {
            this->extract_m(true);
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    auto extract_m(bool exported) -> void
    {
        std::string_view word = this->identifier();

        if (word == "module")
        {
            this->extract_module(exported);
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    auto extract_module(bool exported) -> void
    {
        std::string_view name = this->extract_module_token();

        if (this->current_char() == ';')
        {
            this->skip_one();
            this->push_token(ModuleToken{
                .name = std::string{name.data(), name.size()},
                .exported = exported});
        }
        else if (this->current_char() == ':')
        {
            this->extract_module_partition(name, exported);
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    auto extract_module_partition(std::string_view moduleName, bool exported) -> void
    {
        this->skip_one();
        std::string_view partition = this->extract_module_token();

        if (this->current_char() == ';')
        {
            this->skip_one();
            this->push_token(ModulePartitionToken{
                .mod = std::string{moduleName.data(), moduleName.size()},
                .name = std::string{partition.data(), partition.size()},
                .exported = exported});
        }
        else
        {
            this->skip_semicolon_newline();
        }
    }

    [[nodiscard]] auto extract_module_token() noexcept -> std::string_view
    {
        this->skip_space_comment_newline();
        std::string_view name = this->identifier();
        this->skip_space_comment_newline();
        return name;
    }

    std::size_t pos = 0;
    std::vector<Token> tokens;
};
}
