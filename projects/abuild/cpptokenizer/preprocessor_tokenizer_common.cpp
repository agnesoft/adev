#ifndef __clang__
module abuild.cpptokenizer:preprocessor_tokenizer_common;
import :tokenizer_common;
#endif

namespace abuild
{
//! \private
class PreprocessorTokenizerCommon : public TokenizerCommon
{
public:
    using TokenizerCommon::TokenizerCommon;

protected:
    [[nodiscard]] auto macro_bracket_value() noexcept -> std::string_view
    {
        this->skip_one();
        const std::string_view val = this->macro_value();

        if (this->current_char() == ')')
        {
            this->skip_one();
            return val;
        }

        this->skip_macro();
        return {};
    }

    [[nodiscard]] auto macro_identifier() noexcept -> std::string_view
    {
        this->skip_space_comment_macronewline();
        std::string_view name = this->identifier();
        this->skip_space_comment_macronewline();
        return name;
    }

    [[nodiscard]] auto macro_value() noexcept -> std::string_view
    {
        this->skip_space_comment_macronewline();
        const std::string_view val = this->value();
        this->skip_space_comment_macronewline();
        return val;
    }

    auto skip_macro() noexcept -> void
    {
        while (!this->at_end())
        {
            if (this->is_line_comment() || this->current_char() == '\\')
            {
                this->skip_line();
            }
            else if (this->is_multiline_comment())
            {
                this->skip_multiline_comment();
            }
            else if (this->is_end_of_line())
            {
                return;
            }
            else
            {
                this->skip_one();
            }
        }
    }

    auto skip_space_comment_macronewline() noexcept -> void
    {
        while (!this->at_end())
        {
            if (this->is_line_comment() || this->current_char() == '\\')
            {
                this->skip_line();
            }
            else if (this->is_multiline_comment())
            {
                this->skip_multiline_comment();
            }
            else if (this->is_space())
            {
                this->skip_one();
            }
            else
            {
                return;
            }
        }
    }
};
}
