#ifndef __clang__
module abuild.cpptokenizer : preprocessor_directive;
import : define_directive;
#endif

namespace abuild
{
class PreprocessorDirective : public PreprocessorBase
{
public:
    using PreprocessorBase::PreprocessorBase;

    [[nodiscard]] auto extract(Token &token) -> bool
    {
        this->advance();
        this->skip_whitespace_comment_and_macro_newline();

        if (this->current_view(DEFINE.size()) == DEFINE)
        {
            this->advance(DEFINE.size());
            return DefineDirective{this->source(), this->pos()}.extract(token);
        }

        return false;
    }

private:
    static constexpr std::string_view DEFINE = "define";
};
}
