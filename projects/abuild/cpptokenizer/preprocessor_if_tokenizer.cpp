#ifndef __clang__
module abuild.cpptokenizer : preprocessor_if_tokenizer;
import : preprocessor_tokenizer_common;
#endif

namespace abuild
{
//! \private
class PreprocessorIfTokenizer : public PreprocessorTokenizerCommon
{
public:
    using PreprocessorTokenizerCommon::PreprocessorTokenizerCommon;

    auto tokenize() -> void
    {
        this->skip_space_comment_macronewline();

        while (!this->at_end() && !this->is_end_of_line())
        {
            this->if_directive_element();
        }

        this->push_token(std::move(this->token));
    }

private:
    template<typename T>
    auto push_if_element() -> void
    {
        this->skip_one();
        this->skip_space_comment_macronewline();
        this->token.elements.emplace_back(T{});
    }

    auto if_directive_element() -> void
    {
        if (this->current_char() == '!')
        {
            this->push_if_element<NotToken>();
        }
        else if (this->current_char() == '(')
        {
            this->push_if_element<LeftBracketToken>();
        }
        else if (this->current_char() == ')')
        {
            this->push_if_element<RightBracketToken>();
        }
        else if (this->is_logical_and())
        {
            this->skip_one();
            this->push_if_element<AndToken>();
        }
        else if (this->is_logical_or())
        {
            this->skip_one();
            this->push_if_element<OrToken>();
        }
        else
        {
            this->condition();
        }
    }

    auto condition() -> void
    {
        const std::string_view left = this->value();

        if (left == "defined")
        {
            this->defined();
        }
        else
        {
            this->conditional_expression(left);
        }
    }

    template<typename T>
    auto conditional_expression(std::string_view left) -> void
    {
        const std::string_view right = this->macro_value();
        this->token.elements.emplace_back(T{
            .left = std::string(left.data(), left.size()),
            .right = std::string(right.data(), right.size())});
    }

    template<typename T, typename TOrEquals>
    auto conditional_expression(std::string_view left) -> void
    {
        this->skip_one();

        if (this->current_char() == '=')
        {
            this->skip_one();
            this->conditional_expression<TOrEquals>(left);
        }
        else
        {
            this->conditional_expression<T>(left);
        }
    }

    auto conditional_expression(std::string_view left) -> void
    {
        this->skip_space_comment_macronewline();

        if (this->is_equals_equals())
        {
            this->conditional_expression_equals(left);
        }
        else if (this->is_not_equals())
        {
            this->conditional_expression_not_equals(left);
        }
        else if (this->current_char() == '<')
        {
            this->conditional_expression<LessThanToken, LessThanOrEqualsToken>(left);
        }
        else if (this->current_char() == '>')
        {
            this->conditional_expression<GreaterThanToken, GreaterThanOrEqualsToken>(left);
        }
        else
        {
            this->token.elements.emplace_back(NotToken{});
            this->token.elements.emplace_back(EqualsToken{
                .left = std::string(left.data(), left.size()),
                .right = std::string("0")});
        }
    }

    auto conditional_expression_equals(std::string_view left) -> void
    {
        this->skip(2);
        this->conditional_expression<EqualsToken>(left);
    }

    auto conditional_expression_not_equals(std::string_view left) -> void
    {
        this->token.elements.emplace_back(NotToken{});
        this->conditional_expression_equals(left);
    }

    auto defined() -> void
    {
        const std::string_view defineName = this->macro_bracket_value();
        this->token.elements.emplace_back(DefinedToken{
            .name = std::string(defineName.data(), defineName.size())});
    }

    IfToken token;
};
}
