#ifndef __clang__
module abuild.cpptokenizer : preprocessor_base;
import : tokenizer_base;
#endif

namespace abuild
{
class PreprocessorBase : public TokenizerBase
{
public:
    using TokenizerBase::TokenizerBase;

protected:
    [[nodiscard]] auto extract_name() -> std::string_view
    {
        const std::size_t begin = this->pos();

        while (!this->at_end() && !this->is_current_char_space())
        {
            this->advance();
        }

        return std::string_view(&this->char_at(begin), this->pos() - begin);
    }

    auto skip_whitespace_comment_and_macro_newline() -> void
    {
        do
        {
            if (this->current_char() == '/')
            {
                this->skip_comment();
            }
            else if (this->current_char() == '\\')
            {
                this->skip_line();
            }
            else if (this->is_current_char_space())
            {
                this->advance();
            }
            else
            {
                return;
            }
        }
        while (!this->at_end());
    }

private:
    static constexpr std::string_view DEFINE = "define";
};
}
