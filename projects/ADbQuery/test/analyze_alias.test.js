import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("native alias", () => {
    const data = {
        Id: "int64",
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("custom type alias", () => {
    const data = {
        SomeType: {},
        Id: "SomeType",
    };

    const ast = {
        SomeType: {
            type: "object",
            name: "SomeType",
            functions: [],
            fields: [],
            base: undefined,
        },
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "SomeType",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("non existing alias", () => {
    const data = {
        Id: "SomeType",
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: unknown type 'SomeType' aliased as 'Id'."
    );
});
