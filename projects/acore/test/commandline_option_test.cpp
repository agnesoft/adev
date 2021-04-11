import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

namespace atest
{
auto operator<<(std::ostream &stream, const typename std::vector<std::string>::const_iterator &iterator) -> std::ostream &
{
    stream << *iterator;
    return stream;
}

auto operator<<(std::ostream &stream, [[maybe_unused]] const std::monostate &monostate) -> std::ostream &
{
    stream << "std::monostate";
    return stream;
}

auto operator<<(std::ostream &stream, [[maybe_unused]] const acore::CommandLineOption::DefaultValue &defaultValue) -> std::ostream &
{
    std::visit([&](auto &&value) {
        using DefaultT = std::remove_pointer_t<std::decay_t<decltype(value)>>;

        stream << value;
    },
               defaultValue);

    return stream;
}

auto operator<<(std::ostream &stream, [[maybe_unused]] const acore::CommandLineOption::BoundValue &boundValue) -> std::ostream &
{
    std::visit([&](auto &&value) {
        using BoundT = std::remove_pointer_t<std::decay_t<decltype(value)>>;

        stream << value;
    },
               boundValue);

    return stream;
}
}

static const auto s = suite("acore::CommandLineOption", [] {
#ifdef _MSC_VER
    using ::type_info;
#endif

    test("type traits", [] {
        expect(std::is_default_constructible_v<acore::CommandLineOption>).toBe(true);
        expect(std::is_copy_constructible_v<acore::CommandLineOption>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<acore::CommandLineOption>).toBe(true);
        expect(std::is_copy_assignable_v<acore::CommandLineOption>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<acore::CommandLineOption>).toBe(true);
        expect(std::is_nothrow_destructible_v<acore::CommandLineOption>).toBe(true);
    });

    test("constructor", [] {
        acore::CommandLineOption{};
    });

    test("copy constructor", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        acore::CommandLineOption other{option};

        expect(option.longName()).toBe(std::string("longName1"));
        expect(other.longName()).toBe(std::string("longName1"));
    });

    test("move constructor", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        acore::CommandLineOption other{std::move(option)};

        expect(noexcept(acore::CommandLineOption{std::move(option)})).toBe(true);
        expect(other.longName()).toBe(std::string("longName1"));
    });

    test("bindTo (positional)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
    });

    test("bindTo (named)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
    });

    test("bindTo (defaulted)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::string{"someValue"}).description("").bindTo(&value);
    });

    test("bindTo (repeated)", [] {
        acore::CommandLineOption option;
        std::vector<double> val;
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&val);
    });

    test("bindTo (defaulted, wrong type)", [] {
        acore::CommandLineOption option;
        std::string value;

        const std::string exceptionText = std::string{"The option longName1 default value is set with incompatible type ("} + typeid(std::int64_t).name() + ") to the one it is being bound to (" + typeid(std::string).name() + ").";

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::int64_t{10}).description("").bindTo(&value);
        })
            .toThrow<std::runtime_error>(exceptionText);
    });

    test("boundValue (positional)", [] {
        std::string value;
        acore::CommandLineOption::BoundValue boundValue{&value};
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValue()).toBe(boundValue);
        expect(value).toBe("");
    });

    test("boundValue (named)", [] {
        std::string value;
        acore::CommandLineOption::BoundValue boundValue{&value};
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(std::as_const(option).boundValue()).toBe(boundValue);
        expect(value).toBe("");
    });

    test("boundValue (defaulted)", [] {
        std::string value;
        acore::CommandLineOption::BoundValue boundValue{&value};
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::string{"someValue"}).description("").bindTo(&value);

        expect(std::as_const(option).boundValue()).toBe(boundValue);

        option.defaultBoundValue();

        expect(value).toBe("someValue");
    });

    test("boundValue (repeated)", [] {
        std::string value;
        acore::CommandLineOption::BoundValue boundValue{&value};
        acore::CommandLineOption option;
        std::vector<std::string> val;
        acore::CommandLineOption::BoundValue boundVal{&val};
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(boundVal);

        expect(std::as_const(option).boundValue()).toBe(boundVal);

        option.defaultBoundValue();

        expect(val).toBe(std::vector<std::string>{});
    });

    test("boundValue (defaulted, repeated)", [] {
        std::string value;
        acore::CommandLineOption::BoundValue boundValue{&value};
        acore::CommandLineOption option;
        std::vector<std::string> val;
        acore::CommandLineOption::BoundValue boundVal{&val};
        acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::vector<std::string>{"some", "value"}).description("").bindTo(boundVal);

        expect(std::as_const(option).boundValue()).toBe(boundVal);

        option.defaultBoundValue();

        expect(val).toBe(std::vector<std::string>{"some", "value"});
    });

    test("boundValueAsString (bool)", [] {
        acore::CommandLineOption option;
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("switch");
    });

    test("boundValueAsString (double)", [] {
        acore::CommandLineOption option;
        double value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("floating");
    });

    test("boundValueAsString (std::int64_t)", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("integer");
    });

    test("boundValueAsString (std::string)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("string");
    });

    test("boundValueAsString (std::vector<std::int64_t>)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("integer list");
    });

    test("boundValueAsString (std::vector<double>)", [] {
        acore::CommandLineOption option;
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("floating list");
    });

    test("boundValueAsString (std::vector<std::string>)", [] {
        acore::CommandLineOption option;
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).boundValueAsString()).toBe("string list");
    });

    test("boundValueAsString (undefined)", [] {
        acore::CommandLineOption option;

        expect(std::as_const(option).boundValueAsString()).toBe("undefined");
    });

    test("defaultBoundValue (defaulted)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"def value"}).description("").bindTo(&value);
        option.defaultBoundValue();

        expect(value).toBe("def value");
    });

    test("defaultBoundValue (non defaulted)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        option.defaultBoundValue();

        expect(value).toBe(std::string{});
    });

    test("defaultBoundValue (non bound)", [] {
        acore::CommandLineOption option;
        static_cast<void>(acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"some value"}));

        expect([&] {
            option.defaultBoundValue();
        })
            .toThrow<std::runtime_error>("The option [positional] is missing a bound value.");
    });

    test("defaultValue (defaulted)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"option"}).description("").bindTo(&value);

        expect(std::as_const(option).defaultValue()).toBe(acore::CommandLineOption::DefaultValue{std::string{"option"}});
    });

    test("defaultValue (non defaulted)", [] {
        acore::CommandLineOption option;
        std::string value;
        const acore::CommandLineOption other;

        expect(std::as_const(other).defaultValue()).toBe(acore::CommandLineOption::DefaultValue{});
    });

    test("defaultValue (value)", [] {
        acore::CommandLineOption option;
        acore::CommandLineOption::DefaultValue defValue{std::string{"option"}};

        expect(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().defaultValue(std::move(defValue))))).toBe(true);
    });

    test("defaultValueAsString (true)", [] {
        acore::CommandLineOption option;
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(true).description("").bindTo(&value);

        expect(std::as_const(option).defaultValueAsString()).toBe("true");
    });

    test("defaultValueAsString (false)", [] {
        acore::CommandLineOption option;
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(false).description("").bindTo(&value);

        expect(std::as_const(option).defaultValueAsString()).toBe("false");
    });

    test("defaultValueAsString (double)", [] {
        acore::CommandLineOption option;
        double value = 0;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(12.1).description("").bindTo(&value);

        expect(std::as_const(option).defaultValueAsString()).toBe("12.1");
    });

    test("defaultValueAsString (std::int64_t)", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::int64_t{10}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        expect(std::as_const(option).defaultValueAsString()).toBe("10");
    });

    test("defaultValueAsString (std::string)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"someValue"}).description("").bindTo(&value);

        expect(std::as_const(option).defaultValueAsString()).toBe("someValue");
    });

    test("defaultValueAsString (std::vector<double>)", [] {
        acore::CommandLineOption option;
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<double>{-1.1, 13.3}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        expect(std::as_const(option).defaultValueAsString()).toBe("[-1.1, 13.3]");
    });

    test("defaultValueAsString (std::vector<std::int64_t>)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<std::int64_t>{10, 100}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        expect(std::as_const(option).defaultValueAsString()).toBe("[10, 100]");
    });

    test("defaultValueAsString (std::vector<std::string>)", [] {
        acore::CommandLineOption option;
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<std::string>{"some", "value", "and", "then", "some"}).description("").bindTo(&value);

        expect(std::as_const(option).defaultValueAsString()).toBe("[some, value, and, then, some]");
    });

    test("description", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("My fancty description").bindTo(&value);

        expect(noexcept(std::as_const(option).description())).toBe(true);
        expect(std::as_const(option).description()).toBe("My fancty description");
    });

    test("description (std::string)", [] {
        acore::CommandLineOption option;
        std::string desc = "My fancty description";

        expect(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().description(std::move(desc))))).toBe(true);
    });

    test("isDefaulted (non defaulted", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(noexcept(std::as_const(option).isDefaulted())).toBe(true);
        expect(std::as_const(option).isDefaulted()).toBe(false);
    });

    test("isDefaulted (defaulted)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::int64_t{10}).description("").bindTo(&value);

        expect(std::as_const(option).isDefaulted()).toBe(true);
    });

    test("isPositional (positional)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(noexcept(std::as_const(option).isPositional())).toBe(true);
        expect(std::as_const(option).isPositional()).toBe(true);
    });

    test("isPositional (named)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(std::as_const(option).isPositional()).toBe(false);
    });

    test("isRepeated (repeated)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(noexcept(option.isRepeated())).toBe(true);
        expect(option.isRepeated()).toBe(true);
    });

    test("isRepeated (non repeated)", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(option.isRepeated()).toBe(false);
    });

    test("isRequired (positional)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().required().description("").bindTo(&value);

        expect(noexcept(std::as_const(option).isRequired())).toBe(true);
        expect(std::as_const(option).isRequired()).toBe(true);
    });

    test("isRequired (named)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").required().description("").bindTo(&value);

        expect(std::as_const(option).isRequired()).toBe(true);
    });

    test("isRequired (non required)", [] {
        std::int64_t value = 0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).isRequired()).toBe(false);
    });

    test("isSwitch (switch)", [] {
        acore::CommandLineOption option;
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(std::as_const(option).isSwitch()).toBe(true);
    });

    test("isSwitch (non switch)", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(std::as_const(option).isSwitch()).toBe(false);
    });

    test("longName (positional)", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(noexcept(std::as_const(option).longName())).toBe(true);
        expect(std::as_const(option).longName()).toBe(std::string{});
    });

    test("longName (named)", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&value);

        expect(option.longName()).toBe("longName1");
    });

    test("longName (invalid, start with number)", [] {
        acore::CommandLineOption option;

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("1longName");
        })
            .toThrow<std::runtime_error>("Long name '1longName' is not a valid name ([a-zA-Z][a-zA-Z\\d]+).");
    });

    test("longName (invalid, single character)", [] {
        acore::CommandLineOption option;

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("l");
        })
            .toThrow<std::runtime_error>("Long name 'l' is not a valid name ([a-zA-Z][a-zA-Z\\d]+).");
    });

    test("longName (invalid, end with special character)", [] {
        acore::CommandLineOption option;

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("longName#");
        })
            .toThrow<std::runtime_error>("Long name 'longName#' is not a valid name ([a-zA-Z][a-zA-Z\\d]+).");
    });

    test("match (unmatched)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName", "value"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(false);
        expect(arg).toBe(arguments.cbegin());
        expect(value).toBe(std::string{});
    });

    test("match (--longname value)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName").description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName", "\"quoted value and then some\""};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(arg).toBe(arguments.cend());
        expect(value).toBe("quoted value and then some");
    });

    test("match (--longname=value", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName").shortName('l').description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName=\"a value\""};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe("a value");
    });

    test("match (positional switch)", [] {
        acore::CommandLineOption option;
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"positional"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(true);
    });

    test("match (positional value)", [] {
        acore::CommandLineOption option;
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"value"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe("value");
    });

    test("match (std::int64_t", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"10"};
        auto arg = arguments.cbegin();
        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(10);
    });

    test("match (negative int)", [] {
        acore::CommandLineOption option;
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-10"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(-10);
    });

    test("match (double)", [] {
        acore::CommandLineOption option;
        double value = 0.0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"15.5"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(15.5);
    });

    test("match (negative double)", [] {
        acore::CommandLineOption option;
        double value = 0.0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-15.5"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(-15.5);
    });

    test("match (repeated std::int64_t)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=-10"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(std::vector<std::int64_t>{-10});
    });

    test("match (repeated double)", [] {
        acore::CommandLineOption option;
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-15.5", "20.77", "-1.0"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(std::vector<double>{-15.5, 20.77, -1.0});
    });

    test("match (repeated std::string)", [] {
        acore::CommandLineOption option;
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"-r", "someValue", "-r=\"my Val\""};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(option.match(&arg, arguments.cend())).toBe(true);
        expect(value).toBe(std::vector<std::string>{"someValue", "my Val"});
    });

    test("match (type mismatch)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=hello"};
        auto arg = arguments.cbegin();

        const std::string exceptionText = std::string{"Failed to set option 'repeated' ("} + typeid(value).name() + ") from value 'hello'.";

        expect([&] {
            option.match(&arg, arguments.cend());
        })
            .toThrow<std::runtime_error>(exceptionText);
    });

    test("match (type mismatch, positional)", [] {
        acore::CommandLineOption option;
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=hello"};
        auto arg = arguments.cbegin();

        expect(option.match(&arg, arguments.cend())).toBe(false);
        expect(value).toBe(std::vector<std::int64_t>{});
    });

    test("match (non bound value)", [] {
        acore::CommandLineOption option;
        static_cast<void>(acore::CommandLineOption::Option{&option}.positional().description(""));
        std::vector<std::string> arguments{"value"};
        auto arg = arguments.cbegin();

        const std::string exceptionText = "Bind value undefined for option '[positional]'.";

        expect([&] {
            option.match(&arg, arguments.cend());
        })
            .toThrow<std::runtime_error>(exceptionText);
    });

    test("name (positional)", [] {
        double value = 0.0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);

        expect(option.name()).toBe("[positional]");
    });

    test("name (named)", [] {
        double value = 0.0;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);

        expect(option.name()).toBe("longName1");
    });

    test("required", [] {
        acore::CommandLineOption option;

        expect(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().required()))).toBe(true);
    });

    test("shortName (valid)", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').required().description("").bindTo(&value);

        expect(option.shortName()).toBe('l');
    });

    test("shortName (invalid, number)", [] {
        acore::CommandLineOption option;

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("longName1").shortName('1');
        })
            .toThrow<std::runtime_error>("Short name '1' is not a valid name ([a-zA-Z]).");
    });

    test("shortName (invalid, special character)", [] {
        acore::CommandLineOption option;

        expect([&] {
            acore::CommandLineOption::Option{&option}.longName("longName1").shortName('#');
        })
            .toThrow<std::runtime_error>("Short name '#' is not a valid name ([a-zA-Z]).");
    });

    test("copy assignment", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        acore::CommandLineOption other;
        other = {option};

        expect(option.longName()).toBe(std::string("longName1"));
        expect(other.longName()).toBe(std::string("longName1"));
    });

    test("move assignment", [] {
        std::string value;
        acore::CommandLineOption option;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        acore::CommandLineOption other;
        other = {std::move(option)};

        expect(noexcept(acore::CommandLineOption{std::move(option)})).toBe(true);
        expect(other.longName()).toBe(std::string("longName1"));
    });
});
