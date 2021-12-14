#ifndef __clang__
module abuild.cpptokenizer : tokenizer;
import : preprocessor_directive;
#endif

namespace abuild
{
class Tokenizer : public TokenizerBase
{
public:
    explicit Tokenizer(std::string_view source) :
        TokenizerBase{source, this->sourcePos}
    {
    }

    [[nodiscard]] auto next(Token &token) -> bool
    {
        while (!this->at_end())
        {
            this->skip_space_and_comment();

            if (this->current_char() == '#')
            {
                if (PreprocessorDirective{this->source(), this->pos()}.extract(token))
                {
                    return true;
                }
            }
        }

        return false;
    }

private:
    std::size_t sourcePos = 0;
};
}
