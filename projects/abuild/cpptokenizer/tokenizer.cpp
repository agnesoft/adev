#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : preprocessor_tokenizer;
#endif

namespace abuild
{
//! \private
class Tokenizer : public TokenizerCommon
{
public:
    explicit Tokenizer(std::string_view source) :
        TokenizerCommon{source, this->pos, this->tokens}
    {
    }

    [[nodiscard]] auto tokenize() -> std::vector<Token>
    {
        while (!this->at_end())
        {
            this->skip_space_comment_newline();

            if (this->current_char() == '#')
            {
                PreprocessorTokenizer{this->source_view(), this->pos_ref(), this->tokens_ref()}.tokenize();
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
    std::size_t pos = 0;
    std::vector<Token> tokens;
};
}
