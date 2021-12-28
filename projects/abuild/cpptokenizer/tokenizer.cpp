#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : token;
#endif

namespace abuild
{
//! \private
class Tokenizer
{
public:
    explicit Tokenizer(std::string_view source) :
        source{source}
    {
    }

    [[nodiscard]] auto tokenize() -> std::vector<Token>
    {
        while (!this->at_end())
        {
            this->skip_whitespace_and_comment();

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

        return tokens;
    }

private:
    [[nodiscard]] auto at_end() const noexcept -> bool
    {
        return this->pos == this->source.size();
    }

    [[nodiscard]] auto at(std::size_t p) const noexcept -> const char &
    {
        return this->source[p];
    }

    [[nodiscard]] auto current_char() const noexcept -> const char &
    {
        return this->at(this->pos);
    }

    auto define() -> void
    {
        this->skip_space_comment_and_macro_newline();
        const std::string_view defineName = this->identifier();
        this->skip_space_comment_and_macro_newline();
        const std::string_view value = this->value();
        this->skip_space_and_comment();

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
        this->skip_space_comment_and_macro_newline();
        const std::string_view defineName = this->defined_identifier();
        token.elements.emplace_back(DefinedToken{
            .name = std::string(defineName.data(), defineName.size())});
    }

    [[nodiscard]] auto defined_identifier() noexcept -> std::string_view
    {
        if (this->current_char() == '(')
        {
            this->skip_bracket();
            this->skip_space_comment_and_macro_newline();

            const std::string_view defineName = this->identifier();

            this->skip_space_comment_and_macro_newline();

            if (this->current_char() == ')')
            {
                this->skip_bracket();
                this->skip_space_and_comment();

                return defineName;
            }
        }

        this->skip_macro();
        return {};
    }

    auto endif() -> void
    {
        this->push_token(EndIfToken{});
        this->skip_space_and_comment();
    }

    [[nodiscard]] auto extract_skip_sequence() noexcept -> std::string_view
    {
        this->lexemeBegin = this->pos + 1;

        while (!this->at_end())
        {
            this->skip_one();

            if (this->current_char() == '(')
            {
                std::string_view sequence = this->lexeme();
                this->skip_one();
                return sequence;
            }
        }

        return {};
    }

    auto if_directive() -> void
    {
        this->skip_space_comment_and_macro_newline();

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
        this->skip_space_comment_and_macro_newline();
        const std::string_view defineName = this->identifier();
        this->push_token(IfToken{
            .elements = {DefinedToken{
                .name = std::string(defineName.data(), defineName.size())}}});
        this->skip_space_and_comment();
    }

    auto ifndef() -> void
    {
        this->skip_space_comment_and_macro_newline();
        const std::string_view defineName = this->identifier();
        this->push_token(IfToken{
            .elements = {NotToken{},
                         DefinedToken{
                             .name = std::string(defineName.data(), defineName.size())}}});
        this->skip_space_and_comment();
    }

    [[nodiscard]] auto identifier() noexcept -> std::string_view
    {
        this->lexemeBegin = this->pos;
        this->skip_identifier();
        return this->lexeme();
    }

    [[nodiscard]] auto identifier_or_value() noexcept -> std::string_view
    {
        this->lexemeBegin = this->pos;

        if (this->current_char() == '"')
        {
            this->skip_string();
        }
        else
        {
            this->skip_identifier();
        }

        return this->lexeme();
    }

    [[nodiscard]] auto is_end_of_line() const noexcept -> bool
    {
        return this->current_char() == '\n';
    }

    [[nodiscard]] auto is_equals_equals() const noexcept -> bool
    {
        return this->current_char() == '=' && this->peek() == '=';
    }

    [[nodiscard]] auto is_identifier() const noexcept -> bool
    {
        const auto c = static_cast<unsigned char>(this->current_char());
        return c == '_' || std::isalnum(c) != 0;
    }

    [[nodiscard]] auto is_line_comment() const noexcept -> bool
    {
        return this->current_char() == '/' && this->peek() == '/';
    }

    [[nodiscard]] auto is_logical_and() const noexcept -> bool
    {
        return this->current_char() == '&' && this->peek() == '&';
    }

    [[nodiscard]] auto is_logical_or() const noexcept -> bool
    {
        return this->current_char() == '|' && this->peek() == '|';
    }

    [[nodiscard]] auto is_multiline_comment() const noexcept -> bool
    {
        return this->current_char() == '/' && this->peek() == '*';
    }

    [[nodiscard]] auto is_multiline_comment_end() const noexcept -> bool
    {
        return this->current_char() == '*' && this->peek() == '/';
    }

    [[nodiscard]] auto is_not_equals() const noexcept -> bool
    {
        return this->current_char() == '!' && this->peek() == '=';
    }

    [[nodiscard]] auto is_quote() const noexcept -> bool
    {
        return this->current_char() == '"' && this->previous_character() != '\\';
    }

    [[nodiscard]] auto is_space() const noexcept -> bool
    {
        return this->current_char() == ' '
            || this->current_char() == '\t'
            || this->current_char() == '\r'
            || this->current_char() == '\v'
            || this->current_char() == '\f';
    }

    [[nodiscard]] auto is_whitespace() const noexcept -> bool
    {
        return this->is_space() || this->is_end_of_line();
    }

    [[nodiscard]] auto lexeme() const noexcept -> std::string_view
    {
        return {&this->at(this->lexemeBegin), this->pos - this->lexemeBegin};
    }

    auto macro_condition(IfToken &token) -> void
    {
        const std::string_view left = this->identifier_or_value();

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
        this->skip_space_comment_and_macro_newline();
        const std::string_view right = this->identifier_or_value();
        this->skip_space_comment_and_macro_newline();
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
        this->skip_space_comment_and_macro_newline();

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
        this->skip_space_comment_and_macro_newline();
        token.elements.emplace_back(LeftBracketToken{});
    }

    auto macro_logical_and(IfToken &token) -> void
    {
        this->skip(2);
        this->skip_space_comment_and_macro_newline();
        token.elements.emplace_back(AndToken{});
    }

    auto macro_logical_or(IfToken &token) -> void
    {
        this->skip(2);
        this->skip_space_comment_and_macro_newline();
        token.elements.emplace_back(OrToken{});
    }

    auto macro_negation(IfToken &token) -> void
    {
        token.elements.emplace_back(NotToken{});
        this->skip_one();
        this->skip_space_comment_and_macro_newline();
    }

    auto macro_right_bracket(IfToken &token) -> void
    {
        this->skip_one();
        this->skip_space_comment_and_macro_newline();
        token.elements.emplace_back(RightBracketToken{});
    }

    [[nodiscard]] auto match_sequence(std::string_view sequence) const noexcept -> bool
    {
        return this->at(this->pos - sequence.size() - 1) == ')'
            && std::string_view{&this->at(this->pos - sequence.size()), sequence.size()} == sequence;
    }

    [[nodiscard]] auto peek() const noexcept -> const char &
    {
        return this->at(this->pos + 1);
    }

    auto preprocessor() -> void
    {
        this->skip_hash();
        this->skip_space_comment_and_macro_newline();

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

    [[nodiscard]] auto previous_character() const noexcept -> const char &
    {
        if (this->pos == 0)
        {
            return this->at(0);
        }

        return this->at(this->pos - 1);
    }

    auto push_token(Token token) -> void
    {
        this->tokens.emplace_back(std::move(token));
    }

    auto skip(std::size_t count) noexcept -> void
    {
        this->pos += count;
    }

    auto skip_bracket() noexcept -> void
    {
        this->skip_one();
    }

    auto skip_hash() noexcept -> void
    {
        this->skip_one();
    }

    auto skip_identifier() -> void
    {
        while (!this->at_end() && this->is_identifier())
        {
            this->skip_one();
        }
    }

    auto skip_line() noexcept -> void
    {
        this->skip_one();

        while (!this->at_end())
        {
            if (this->is_end_of_line())
            {
                this->skip_one();
                return;
            }

            this->skip_one();
        };
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

    auto skip_multiline_comment() noexcept -> void
    {
        this->skip_multiline_quote_start();

        while (!this->at_end())
        {
            if (this->is_multiline_comment_end())
            {
                this->skip_multiline_quote_end();
                return;
            }

            this->skip_one();
        }
    }

    auto skip_multiline_quote_end() noexcept -> void
    {
        this->skip(2);
    }

    auto skip_multiline_quote_start() noexcept -> void
    {
        this->skip(2);
    }

    auto skip_one() noexcept -> void
    {
        this->skip(1);
    }

    auto skip_quote() noexcept -> void
    {
        this->skip_one();
    }

    auto skip_space_and_comment() noexcept -> void
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

    auto skip_whitespace_and_comment() noexcept -> void
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
            else if (this->is_whitespace())
            {
                this->skip_one();
            }
            else
            {
                return;
            }
        }
    }

    auto skip_space_comment_and_macro_newline() noexcept -> void
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

    auto skip_raw_string() noexcept -> void
    {
        std::string_view skipSequence = this->extract_skip_sequence();

        while (!this->at_end())
        {
            if (this->is_quote() && this->match_sequence(skipSequence))
            {
                this->skip_quote();
                return;
            }

            this->skip_one();
        }
    }

    auto skip_regular_string() noexcept -> void
    {
        this->skip_quote();

        while (!this->at_end())
        {
            if (this->is_quote())
            {
                this->skip_quote();
                return;
            }

            this->skip_one();
        }
    }

    auto skip_string() noexcept -> void
    {
        if (this->previous_character() == 'R')
        {
            this->skip_raw_string();
        }
        else
        {
            this->skip_regular_string();
        }
    }

    auto undef() -> void
    {
        this->skip_space_comment_and_macro_newline();
        const std::string_view defineName = this->identifier();
        this->push_token(UndefToken{
            .name = std::string(defineName.data(), defineName.size())});
        this->skip_space_and_comment();
    }

    [[nodiscard]] auto value() noexcept -> std::string_view
    {
        this->lexemeBegin = this->pos;

        if (this->current_char() == '"')
        {
            this->skip_string();
        }
        else
        {
            this->skip_identifier();
        }

        return this->lexeme();
    }

    std::string_view source;
    std::size_t lexemeBegin = 0;
    std::size_t pos = 0;
    std::vector<Token> tokens;
};
}
