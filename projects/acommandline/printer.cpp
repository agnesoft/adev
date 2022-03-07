#ifndef __clang__
module acommandline : printer;
import : option_help_line;
#endif

namespace acommandline
{
//! \private
class Printer
{
public:
    explicit Printer(std::ostream &stream) :
        stream{stream}
    {
    }

    auto print_help(const std::string &appName, const std::vector<Option> &options) -> void
    {
        this->print_help_header(appName);
        this->print_options(Printer::option_help_lines(options));
    }

private:
    [[nodiscard]] static auto count_name_width(const std::vector<OptionHelpLine> &helpLines) -> int
    {
        static constexpr std::size_t minimumWidth = 10;
        std::size_t width = minimumWidth;

        for (const OptionHelpLine &line : helpLines)
        {
            if (width < line.name().size())
            {
                width = line.name().size();
            }
        }

        return static_cast<int>(width);
    }

    [[nodiscard]] static auto count_attributes_width(const std::vector<OptionHelpLine> &helpLines) -> int
    {
        static constexpr std::size_t minimumWidth = 10;
        std::size_t width = minimumWidth;

        for (const OptionHelpLine &line : helpLines)
        {
            if (width < line.attributes().size())
            {
                width = line.attributes().size();
            }
        }

        return static_cast<int>(width);
    }

    [[nodiscard]] static auto option_help_lines(const std::vector<Option> &options) -> std::vector<OptionHelpLine>
    {
        std::vector<OptionHelpLine> helpLines;
        helpLines.reserve(options.size());

        for (const Option &option : options)
        {
            helpLines.emplace_back(OptionHelpLine{option});
        }

        return helpLines;
    }

    auto print_help_header(const std::string &appName) -> void
    {
        static constexpr int width = 15;

        this->stream << "Usage:\n    " << appName << " [options]" << '\n';
        this->stream << "Syntax:\n";
        this->stream << std::left << std::setw(width) << "  switch"
                     << "--switch, -s\n";
        this->stream << std::left << std::setw(width) << "  named"
                     << "--name=value, --n=value, --name value, --n value\n";
        this->stream << std::left << std::setw(width) << "  positional"
                     << "value\n";
        this->stream << "Options:" << '\n';
    }

    auto print_help_option(int nameWidth, int attributesWidth) -> void
    {
        this->stream << "    " << std::left << std::setw(nameWidth) << "-?" << std::setw(attributesWidth) << "[switch]"
                     << "Prints this help.\n";
    }

    auto print_option(const OptionHelpLine &line, int nameWidth, int attributesWidth) -> void
    {
        this->stream << "    ";
        this->stream << std::left << std::setw(nameWidth) << line.name();
        this->stream << std::setw(attributesWidth) << line.attributes() << line.option().description << '\n';
    }

    auto print_optional(const std::vector<OptionHelpLine> &helpLines, int nameWidth, int attributesWidth) -> void
    {
        this->stream << "  Optional:\n";

        for (const OptionHelpLine &line : helpLines)
        {
            if (!line.option().required && !(line.option().longName == "[positional]"))
            {
                this->print_option(line, nameWidth, attributesWidth);
            }
        }
    }

    auto print_options(const std::vector<OptionHelpLine> &helpLines) -> void
    {
        const int nameWidth = Printer::count_name_width(helpLines) + 5;
        const int attributesWidth = Printer::count_attributes_width(helpLines) + 5;
        this->print_help_option(nameWidth, attributesWidth);
        this->print_required(helpLines, nameWidth, attributesWidth);
        this->print_optional(helpLines, nameWidth, attributesWidth);
        this->print_positional(helpLines, nameWidth, attributesWidth);
    }

    auto print_positional(const std::vector<OptionHelpLine> &helpLines, int nameWidth, int attributesWidth) -> void
    {
        this->stream << "  Positional:\n";

        for (const OptionHelpLine &line : helpLines)
        {
            if (line.option().longName == "[positional]")
            {
                this->print_option(line, nameWidth, attributesWidth);
            }
        }
    }

    auto print_required(const std::vector<OptionHelpLine> &helpLines, int nameWidth, int attributesWidth) -> void
    {
        this->stream << "  Required:\n";

        for (const OptionHelpLine &line : helpLines)
        {
            if (line.option().required)
            {
                this->print_option(line, nameWidth, attributesWidth);
            }
        }
    }

    std::ostream &stream;
};
}
