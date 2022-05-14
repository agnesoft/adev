#ifndef __clang__
module abuild.cpptokenizer:preprocessor_tokenizer;
import :preprocessor_if_tokenizer;
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
        else if (type == "else")
        {
            this->push_token(ElseToken{});
        }
        else if (type == "elif")
        {
            this->push_token(ElseToken{});
            PreprocessorIfTokenizer{this->source_view(), this->pos_ref(), this->tokens_ref()}.tokenize();
        }
        else if (type == "elifdef")
        {
            this->push_token(ElseToken{});
            this->ifdef();
        }
        else if (type == "elifndef")
        {
            this->push_token(ElseToken{});
            this->ifndef();
        }
        else if (type == "endif")
        {
            this->endif();
        }
        else if (type == "if")
        {
            PreprocessorIfTokenizer{this->source_view(), this->pos_ref(), this->tokens_ref()}.tokenize();
        }
        else if (type == "ifdef")
        {
            this->ifdef();
        }
        else if (type == "ifndef")
        {
            this->ifndef();
        }
        else if (type == "include")
        {
            this->include();
        }
        else if (type == "undef")
        {
            this->undef();
        }
        else
        {
            this->skip_macro();
        }
    }

private:
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

    auto endif() -> void
    {
        this->push_token(EndIfToken{});
        this->skip_space_comment();
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
                             .name = std::string(defineName.data(), defineName.size())}}
        });
        this->skip_space_comment();
    }

    template<typename T>
    auto do_include(std::string_view value) -> void
    {
        if (!value.empty())
        {
            this->push_token(T{
                .name = std::string(value.data(), value.size())});
        }

        this->skip_space_comment();
    }

    auto include() -> void
    {
        this->skip_space_comment_macronewline();

        if (this->current_char() == '"')
        {
            this->do_include<IncludeLocalToken>(this->local_include());
        }
        else if (this->current_char() == '<')
        {
            this->do_include<IncludeExternalToken>(this->external_include());
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
