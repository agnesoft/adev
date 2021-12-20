#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : token;
#endif

namespace abuild
{
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
            else if (!this->at_end())
            {
                this->advance();
            }
        }

        return tokens;
    }

private:
    auto advance() noexcept -> void
    {
        this->advance(1);
    }

    auto advance(std::size_t count) noexcept -> void
    {
        this->pos += count;
    }

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

    [[nodiscard]] auto identifier() noexcept -> std::string_view
    {
        this->lexemeBegin = this->pos;
        this->skip_identifier();
        return this->lexeme();
    }

    [[nodiscard]] auto is_end_of_line() const noexcept -> bool
    {
        return this->current_char() == '\n';
    }

    [[nodiscard]] auto is_line_comment() const noexcept -> bool
    {
        return this->current_char() == '/' && this->peek() == '/';
    }

    [[nodiscard]] auto is_multiline_comment() const noexcept -> bool
    {
        return this->current_char() == '/' && this->peek() == '*';
    }

    [[nodiscard]] auto is_multiline_comment_end() const noexcept -> bool
    {
        return this->current_char() == '*' && this->peek() == '/';
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
    }

    [[nodiscard]] auto previous_character() const noexcept -> const char &
    {
        return this->at(this->pos - 1);
    }

    auto push_token(Token token) -> void
    {
        this->tokens.emplace_back(std::move(token));
    }

    auto skip_hash() noexcept -> void
    {
        this->advance();
    }

    auto skip_identifier() -> void
    {
        while (!this->at_end()
               && !this->is_whitespace()
               && !this->is_line_comment()
               && !this->is_multiline_comment())
        {
            this->advance();
        }
    }

    auto skip_line() noexcept -> void
    {
        this->advance();

        while (!this->at_end())
        {
            if (this->is_end_of_line())
            {
                this->advance();
                return;
            }

            this->advance();
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
                this->advance();
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

            this->advance();
        }
    }

    auto skip_multiline_quote_end() noexcept -> void
    {
        this->advance(2);
    }

    auto skip_multiline_quote_start() noexcept -> void
    {
        this->advance(2);
    }

    auto skip_quote() noexcept -> void
    {
        this->advance();
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
                this->advance();
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
                this->advance();
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
                this->advance();
            }
            else
            {
                return;
            }
        }
    }

    auto skip_string() noexcept -> void
    {
        this->skip_quote();

        while (!this->at_end())
        {
            if (this->is_quote())
            {
                this->skip_quote();
                return;
            }

            this->advance();
        }
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
