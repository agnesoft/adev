import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("valid arguments", () => {
    const data = {
        Id: "int64",
        From: "Id",
        foo: {
            arguments: ["Id", "From"],
        },
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
        From: {
            type: "alias",
            name: "From",
            aliasedType: "Id",
        },
        foo: {
            type: "function",
            name: "foo",
            arguments: ["Id", "From"],
            body: [],
            returnValue: undefined,
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("unknown argument type", () => {
    const data = {
        Id: "int64",
        foo: {
            arguments: ["Id", "From"],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        `Analyzer: the argument 'From' in function 'foo' is not an existing type.`
    );
});
