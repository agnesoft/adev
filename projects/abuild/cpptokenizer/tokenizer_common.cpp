#ifndef __clang__
module abuild.cpptokenizer : tokenizer_common;
import : token;
#endif

namespace abuild
{
//! \private
class TokenizerCommon
{
public:
    explicit TokenizerCommon(std::string_view source) :
        source{source}
    {
    }

protected:
    [[nodiscard]] auto at_end() const noexcept -> bool
    {
        return this->pos == this->source.size();
    }

    [[nodiscard]] auto current_char() const noexcept -> const char &
    {
        return this->at(this->pos);
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

    [[nodiscard]] auto is_equals_equals() const noexcept -> bool
    {
        return this->current_char() == '=' && this->peek() == '=';
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

    [[nodiscard]] auto is_not_equals() const noexcept -> bool
    {
        return this->current_char() == '!' && this->peek() == '=';
    }

    [[nodiscard]] auto is_space() const noexcept -> bool
    {
        return this->current_char() == ' '
            || this->current_char() == '\t'
            || this->current_char() == '\r'
            || this->current_char() == '\v'
            || this->current_char() == '\f';
    }

    auto push_token(Token token) -> void
    {
        this->tokens.emplace_back(std::move(token));
    }

    auto skip(std::size_t count) noexcept -> void
    {
        this->pos += count;
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

    auto skip_one() noexcept -> void
    {
        this->skip(1);
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

    auto skip_space_comment_newline() noexcept -> void
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

    [[nodiscard]] auto take_tokens() noexcept -> std::vector<Token>
    {
        return std::move(this->tokens);
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

private:
    [[nodiscard]] auto at(std::size_t p) const noexcept -> const char &
    {
        return this->source[p];
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

    [[nodiscard]] auto is_identifier() const noexcept -> bool
    {
        const auto c = static_cast<unsigned char>(this->current_char());
        return c == '_' || std::isalnum(c) != 0;
    }

    [[nodiscard]] auto is_multiline_comment_end() const noexcept -> bool
    {
        return this->current_char() == '*' && this->peek() == '/';
    }

    [[nodiscard]] auto is_quote() const noexcept -> bool
    {
        return this->current_char() == '"' && this->previous_character() != '\\';
    }

    [[nodiscard]] auto is_whitespace() const noexcept -> bool
    {
        return this->is_space() || this->is_end_of_line();
    }

    [[nodiscard]] auto lexeme() const noexcept -> std::string_view
    {
        return {&this->at(this->lexemeBegin), this->pos - this->lexemeBegin};
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

    [[nodiscard]] auto previous_character() const noexcept -> const char &
    {
        if (this->pos == 0)
        {
            return this->at(0);
        }

        return this->at(this->pos - 1);
    }

    auto skip_identifier() -> void
    {
        while (!this->at_end() && this->is_identifier())
        {
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

    auto skip_raw_string() noexcept -> void
    {
        std::string_view skipSequence = this->extract_skip_sequence();

        while (!this->at_end())
        {
            if (this->is_quote() && this->match_sequence(skipSequence))
            {
                this->skip_one();
                return;
            }

            this->skip_one();
        }
    }

    auto skip_regular_string() noexcept -> void
    {
        this->skip_one();

        while (!this->at_end())
        {
            if (this->is_quote())
            {
                this->skip_one();
                return;
            }

            this->skip_one();
        }
    }

    std::string_view source;
    std::size_t lexemeBegin = 0;
    std::size_t pos = 0;
    std::vector<Token> tokens;
};
}
