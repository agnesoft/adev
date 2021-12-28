#ifndef __clang__
module abuild.cpptokenizer : preprocessor_tokenizer;
import : preprocessor_tokenizer_common;
#endif

namespace abuild
{
//! \private
class PreprocessorTokenizer : public PreprocessorTokenizerCommon
{
public:
    using PreprocessorTokenizerCommon::PreprocessorTokenizerCommon;

    auto tokenize() -> void
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

private:
    template<typename T>
    auto push_if_element(IfToken &token) -> void
    {
        this->skip_one();
        this->skip_space_comment_macronewline();
        token.elements.emplace_back(T{});
    }

    auto define() -> void
    {
        const std::string_view defineName = this->macro_identifier();
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
        const std::string_view defineName = this->defined_identifier();
        token.elements.emplace_back(DefinedToken{
            .name = std::string(defineName.data(), defineName.size())});
    }

    [[nodiscard]] auto defined_identifier() noexcept -> std::string_view
    {
        this->skip_space_comment_macronewline();

        if (this->current_char() == '(')
        {
            this->skip_one();
            const std::string_view defineName = this->macro_identifier();

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
            this->push_if_element<NotToken>(token);
        }
        else if (this->current_char() == '(')
        {
            this->push_if_element<LeftBracketToken>(token);
        }
        else if (this->current_char() == ')')
        {
            this->push_if_element<RightBracketToken>(token);
        }
        else if (this->is_logical_and())
        {
            this->skip_one();
            this->push_if_element<AndToken>(token);
        }
        else if (this->is_logical_or())
        {
            this->skip_one();
            this->push_if_element<OrToken>(token);
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
        const std::string_view right = this->macro_value();
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

    template<typename T, typename TOrEquals>
    auto macro_expression(std::string_view left, IfToken &token) -> void
    {
        this->skip_one();

        if (this->current_char() == '=')
        {
            this->skip_one();
            this->macro_expression<TOrEquals>(left, token);
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
        else
        {
            this->skip_macro();
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
