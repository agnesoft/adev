import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("valid return", () => {
    const data = {
        Id: "int64",
        foo: {
            return: "Id",
        },
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [],
            returnValue: "Id",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("unknown return type", () => {
    const data = {
        foo: {
            return: "Id",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        `Analyzer: the return type 'Id' of function 'foo' is not an existing type.`
    );
});
