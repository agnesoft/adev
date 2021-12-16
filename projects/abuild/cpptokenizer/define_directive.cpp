#ifndef __clang__
module abuild.cpptokenizer : define_directive;
import : preprocessor_base;
#endif

namespace abuild
{
class DefineDirective : public PreprocessorBase
{
public:
    using PreprocessorBase::PreprocessorBase;

    [[nodiscard]] auto extract(Token &token) -> bool
    {
        this->skip_whitespace_comment_and_macro_newline();
        std::string_view name = this->extract_name();
        this->skip_whitespace_comment_and_macro_newline();
        std::string_view value = this->extract_value();

        if (value.empty() || value.back() != '\\')
        {
            token = DefineToken{
                .name = std::string(name.data(), name.size()),
                .value = std::string(value.data(), value.size())};
            return true;
        }

        return false;
    }

private:
    [[nodiscard]] auto extract_value() -> std::string_view
    {
        const std::size_t begin = this->pos();

        if (this->current_char() == '"')
        {
            this->skip_string();
        }
        else
        {
            this->skip_value();
        }

        return TokenizerBase::trim(std::string_view(&this->char_at(begin), this->pos() - begin));
    }

    auto skip_value() -> void
    {
        while (!this->at_end())
        {
            this->advance();

            if (this->current_char() == '\\' || this->current_char() == '\n')
            {
                this->advance();
                break;
            }
        }
    }
};
}
