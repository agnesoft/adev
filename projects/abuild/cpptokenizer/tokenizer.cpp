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

    auto extract_export() -> void
    {
        this->skip_space_comment_newline();

        if (this->current_char() == 'm')
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
        std::string_view name = this->extract_module_name();

        if (!name.empty())
        {
            this->push_token(ModuleToken{
                .name = std::string{name.data(), name.size()},
                .exported = exported});
        }
    }

    [[nodiscard]] auto extract_module_name() noexcept -> std::string_view
    {
        this->skip_space_comment_newline();
        std::string_view name = this->identifier();
        this->skip_space_comment_newline();

        if (this->current_char() == ';')
        {
            this->skip_one();
            return name;
        }

        return {};
    }

    std::size_t pos = 0;
    std::vector<Token> tokens;
};
}
