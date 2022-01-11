import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("module partition", [] { // NOLINT(cert-err58-cpp)
    test("regular", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule : my_partition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(false);
    });

    test("no spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule:my_partition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(false);
    });

    test("extra spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   module    mymodule   :   my_partition  ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(false);
    });

    test("with end of lines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module\nmymodule\n:\nmy_partition\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(false);
    });

    test("with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n\n*/module/*\n\n*/mymodule/*\n\n*/:/*\n\n*/my_partition/*\n\n*/;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(false);
    });

    test("export regular", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export module mymodule : my_partition;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(true);
    });

    test("export with spaces", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("   export   module    mymodule  :   my_partition ;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(true);
    });

    test("export with end of lines", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("export\nmodule\nmymodule\n:\nmy_partition\n;");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(true);
    });

    test("export with comments", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("/*\n*/export/*\n\n*/module/*\n\n*/mymodule/*\n*/:/*\n*/my_partition/*\n*/;/*\n*/");

        assert_(tokens.size()).to_be(1U);
        assert_(std::holds_alternative<::abuild::ModulePartitionToken>(tokens[0])).to_be(true);

        const auto &token = std::get<::abuild::ModulePartitionToken>(tokens[0]);

        expect(token.mod).to_be("mymodule");
        expect(token.name).to_be("my_partition");
        expect(token.exported).to_be(true);
    });

    test("missing semicolon with end of line", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule : mypartition\n\nmodule mymodule : otherpartition;");

        assert_(tokens.size()).to_be(0U);
    });

    test("missing semicolon", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule : my_partition");

        assert_(tokens.size()).to_be(0U);
    });

    test("incorrect space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule : my partition;");

        assert_(tokens.size()).to_be(0U);
    });

    test("empty partition name", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule :;");

        assert_(tokens.size()).to_be(0U);
    });

    test("empty partition name with space", [] {
        const std::vector<::abuild::Token> tokens = ::abuild::tokenize("module mymodule : ;");

        assert_(tokens.size()).to_be(0U);
    });
});
