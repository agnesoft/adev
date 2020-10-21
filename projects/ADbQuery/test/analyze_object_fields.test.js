import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("valid fields", () => {
    const data = {
        Id: "int64",
        Ids: ["Id"],
        MyObj: {
            fields: ["Id", "Ids"],
        },
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
        Ids: {
            type: "array",
            name: "Ids",
            arrayType: "Id",
        },
        MyObj: {
            type: "object",
            name: "MyObj",
            base: undefined,
            functions: [],
            fields: ["Id", "Ids"],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("missing field type", () => {
    const data = {
        Id: "int64",
        MyObj: {
            fields: ["Id", "Ids"],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: the field 'Ids' of object 'MyObj' is not an existing type."
    );
});

test("invalid field type (function)", () => {
    const data = {
        foo: { body: [] },
        MyObj: {
            fields: ["foo"],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: the field 'foo' of object 'MyObj' cannot be a function."
    );
});
