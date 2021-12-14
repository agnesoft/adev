#ifndef __clang__
module abuild.cpptokenizer : tokenizer_base;
import : token;
#endif

namespace abuild
{
class TokenizerBase
{
public:
    TokenizerBase(std::string_view source, std::size_t &pos) :
        sourceView{source},
        position{pos}
    {
    }

protected:
    [[nodiscard]] auto at_end() const noexcept -> bool
    {
        return this->position >= this->sourceView.size();
    }

    auto advance() -> void
    {
        this->position++;
    }

    auto advance(std::size_t count) -> void
    {
        this->position += count;
    }

    [[nodiscard]] auto current_char() const noexcept -> const char &
    {
        return this->sourceView[this->position];
    }

    [[nodiscard]] auto current_view(std::size_t count) const noexcept -> std::string_view
    {
        return std::string_view(&this->sourceView[this->position], std::min(count, this->sourceView.size() - this->position));
    }

    [[nodiscard]] auto is_current_char_space() const -> bool
    {
        return std::isspace(this->current_char()) != 0;
    }

    [[nodiscard]] auto pos() const noexcept -> std::size_t &
    {
        return this->position;
    }

    [[nodiscard]] auto previous_character() const noexcept -> char
    {
        return this->sourceView[this->position - 1];
    }

    auto skip_line() -> void
    {
        while (!this->at_end() && this->current_char() != '\n')
        {
            this->advance();
        }
    }

    auto skip_comment() -> void
    {
        this->advance();

        if (this->current_char() == '/')
        {
            this->skip_line_comment();
        }
        else if (this->current_char() == '*')
        {
            this->skip_multiline_comment();
        }
    }

    auto skip_space_and_comment() -> void
    {
        do
        {
            if (this->current_char() == '/')
            {
                this->skip_comment();
            }
            else if (this->is_current_char_space())
            {
                this->skip_space();
            }
            else
            {
                return;
            }
        }
        while (!this->at_end());
    }

    // auto skip_string() -> void
    // {
    //     do
    //     {
    //         this->advance();

    //         if (this->current_char() == '"')
    //         {
    //             if (this->previous_character() != '\\')
    //             {
    //                 this->advance();
    //                 return;
    //             }
    //         }
    //     }
    //     while (!this->at_end());
    // }

    [[nodiscard]] auto source() const noexcept -> std::string_view
    {
        return this->sourceView;
    }

    [[nodiscard]] static auto trim(std::string_view input) -> std::string_view
    {
        size_t prefix = 0;
        size_t suffix = input.size();

        while (std::isspace(input[prefix]) && prefix < input.size())
        {
            prefix++;
        }

        while (std::isspace(input[suffix - 1]) && 0 < suffix)
        {
            suffix--;
        }

        return std::string_view(&input[prefix], suffix - prefix);
    }

private:
    auto skip_line_comment() -> void
    {
        do
        {
            this->advance();

            if (this->current_char() == '\n')
            {
                this->advance();
                return;
            }
        }
        while (!this->at_end());
    }

    auto skip_multiline_comment() -> void
    {
        do
        {
            this->advance();

            if (this->current_char() == '*')
            {
                this->advance();

                if (this->current_char() == '/')
                {
                    this->advance();
                    return;
                }
            }
        }
        while (!this->at_end());
    }

    auto skip_space() -> void
    {
        do
        {
            this->advance();
        }
        while (this->is_current_char_space());
    }

    std::string_view sourceView;
    std::size_t &position;
};
}
