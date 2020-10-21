import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("valid variants", () => {
    const data = {
        SomeType: {},
        MyVariant: ["SomeType", "int64"],
    };

    const ast = {
        SomeType: {
            type: "object",
            name: "SomeType",
            functions: [],
            fields: [],
            base: undefined,
        },
        MyVariant: {
            type: "variant",
            name: "MyVariant",
            variants: ["SomeType", "int64"],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("missing variant", () => {
    const data = {
        MyVariant: ["int64", "SomeType"],
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: unknown type 'SomeType' used as a variant of 'MyVariant'."
    );
});
