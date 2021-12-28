#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : tokenizer_common;
#endif

namespace abuild
{
//! \private
class Tokenizer : public TokenizerCommon
{
public:
    using TokenizerCommon::TokenizerCommon;

    [[nodiscard]] auto tokenize() -> std::vector<Token>
    {
        while (!this->at_end())
        {
            this->skip_space_comment_newline();

            if (this->current_char() == '#')
            {
                this->preprocessor();
            }
            else if (this->current_char() == '"')
            {
                this->skip_string();
            }
            else if (!this->at_end())
            {
                this->skip_one();
            }
        }

        return this->take_tokens();
    }

private:
    auto define() -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view defineName = this->identifier();
        this->skip_space_comment_macronewline();
        const std::string_view value = this->value();
        this->skip_space_comment();

        if (this->is_end_of_line() || this->at_end())
        {
            this->push_token(DefineToken{
                .name = std::string(defineName.data(), defineName.size()),
                .value = std::string(value.data(), value.size())});
        }
        else
        {
            this->skip_macro();
        }
    }

    auto defined(IfToken &token) -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view defineName = this->defined_identifier();
        token.elements.emplace_back(DefinedToken{
            .name = std::string(defineName.data(), defineName.size())});
    }

    [[nodiscard]] auto defined_identifier() noexcept -> std::string_view
    {
        if (this->current_char() == '(')
        {
            this->skip_one();
            this->skip_space_comment_macronewline();

            const std::string_view defineName = this->identifier();

            this->skip_space_comment_macronewline();

            if (this->current_char() == ')')
            {
                this->skip_one();
                this->skip_space_comment();

                return defineName;
            }
        }

        this->skip_macro();
        return {};
    }

    auto endif() -> void
    {
        this->push_token(EndIfToken{});
        this->skip_space_comment();
    }

    auto if_directive() -> void
    {
        this->skip_space_comment_macronewline();

        IfToken token;

        while (!this->at_end() && !this->is_end_of_line())
        {
            this->if_directive_element(token);
        }

        this->push_token(std::move(token));
    }

    auto if_directive_element(IfToken &token) -> void
    {
        if (this->current_char() == '!')
        {
            this->macro_negation(token);
        }
        else if (this->current_char() == '(')
        {
            this->macro_left_bracket(token);
        }
        else if (this->current_char() == ')')
        {
            this->macro_right_bracket(token);
        }
        else if (this->is_logical_and())
        {
            this->macro_logical_and(token);
        }
        else if (this->is_logical_or())
        {
            this->macro_logical_or(token);
        }
        else
        {
            this->macro_condition(token);
        }
    }

    auto ifdef() -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view defineName = this->identifier();
        this->push_token(IfToken{
            .elements = {DefinedToken{
                .name = std::string(defineName.data(), defineName.size())}}});
        this->skip_space_comment();
    }

    auto ifndef() -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view defineName = this->identifier();
        this->push_token(IfToken{
            .elements = {NotToken{},
                         DefinedToken{
                             .name = std::string(defineName.data(), defineName.size())}}});
        this->skip_space_comment();
    }

    auto macro_condition(IfToken &token) -> void
    {
        const std::string_view left = this->value();

        if (left == "defined")
        {
            this->defined(token);
        }
        else
        {
            this->macro_expression(left, token);
        }
    }

    template<typename T>
    auto macro_expression(std::string_view left, IfToken &token) -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view right = this->value();
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(T{
            .left = std::string(left.data(), left.size()),
            .right = std::string(right.data(), right.size())});
    }

    auto macro_expression_equals(std::string_view left, IfToken &token) -> void
    {
        this->skip(2);
        this->macro_expression<EqualsToken>(left, token);
    }

    auto macro_expression_not_equals(std::string_view left, IfToken &token) -> void
    {
        token.elements.emplace_back(NotToken{});
        this->macro_expression_equals(left, token);
    }

    template<typename T>
    auto macro_expression_or_equals(std::string_view left, IfToken &token) -> void
    {
        this->skip_one();
        this->macro_expression<T>(left, token);
    }

    template<typename T, typename TOrEquals>
    auto macro_expression(std::string_view left, IfToken &token) -> void
    {
        this->skip_one();

        if (this->current_char() == '=')
        {
            this->macro_expression_or_equals<TOrEquals>(left, token);
        }
        else
        {
            this->macro_expression<T>(left, token);
        }
    }

    auto macro_expression(std::string_view left, IfToken &token) -> void
    {
        this->skip_space_comment_macronewline();

        if (this->is_equals_equals())
        {
            this->macro_expression_equals(left, token);
        }
        else if (this->is_not_equals())
        {
            this->macro_expression_not_equals(left, token);
        }
        else if (this->current_char() == '<')
        {
            this->macro_expression<LessThanToken, LessThanOrEqualsToken>(left, token);
        }
        else if (this->current_char() == '>')
        {
            this->macro_expression<GreaterThanToken, GreaterThanOrEqualsToken>(left, token);
        }
        else
        {
            this->skip_macro();
        }
    }

    auto macro_left_bracket(IfToken &token) -> void
    {
        this->skip_one();
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(LeftBracketToken{});
    }

    auto macro_logical_and(IfToken &token) -> void
    {
        this->skip(2);
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(AndToken{});
    }

    auto macro_logical_or(IfToken &token) -> void
    {
        this->skip(2);
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(OrToken{});
    }

    auto macro_negation(IfToken &token) -> void
    {
        token.elements.emplace_back(NotToken{});
        this->skip_one();
        this->skip_space_comment_macronewline();
    }

    auto macro_right_bracket(IfToken &token) -> void
    {
        this->skip_one();
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(RightBracketToken{});
    }

    auto preprocessor() -> void
    {
        this->skip_one();
        this->skip_space_comment_macronewline();

        const std::string_view type = this->identifier();

        if (type == "define")
        {
            this->define();
        }
        else if (type == "undef")
        {
            this->undef();
        }
        else if (type == "ifdef")
        {
            this->ifdef();
        }
        else if (type == "ifndef")
        {
            this->ifndef();
        }
        else if (type == "endif")
        {
            this->endif();
        }
        else if (type == "if")
        {
            this->if_directive();
        }
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

    auto skip_space_comment() noexcept -> void
    {
        while (!this->at_end())
        {
            if (this->is_line_comment())
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

    auto undef() -> void
    {
        this->skip_space_comment_macronewline();
        const std::string_view defineName = this->identifier();
        this->push_token(UndefToken{
            .name = std::string(defineName.data(), defineName.size())});
        this->skip_space_comment();
    }
};
}
