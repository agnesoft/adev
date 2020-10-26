import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("native type array", () => {
    const data = {
        MyArr: ["int64"],
    };

    const ast = {
        MyArr: {
            type: "array",
            name: "MyArr",
            arrayType: "int64",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("custom type array", () => {
    const data = {
        SomeObj: {},
        MyArr: ["SomeObj"],
    };

    const ast = {
        SomeObj: {
            type: "object",
            name: "SomeObj",
            functions: [],
            fields: [],
            base: undefined,
        },
        MyArr: {
            type: "array",
            name: "MyArr",
            arrayType: "SomeObj",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("non existing array type", () => {
    const data = {
        MyArr: ["SomeType"],
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: unknown type 'SomeType' used as an array type of 'MyArr'."
    );
});
