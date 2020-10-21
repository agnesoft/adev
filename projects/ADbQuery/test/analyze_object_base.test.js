import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("empty", () => {
    const data = {
        MyObj: {},
    };

    const ast = {
        MyObj: {
            type: "object",
            name: "MyObj",
            functions: [],
            fields: [],
            base: undefined,
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("valid base", () => {
    const data = {
        BaseObj: {},
        MyObj: { base: "BaseObj" },
    };

    const ast = {
        BaseObj: {
            type: "object",
            name: "BaseObj",
            functions: [],
            fields: [],
            base: undefined,
        },
        MyObj: {
            type: "object",
            name: "MyObj",
            functions: [],
            fields: [],
            base: "BaseObj",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("aliased base", () => {
    const data = {
        BaseObj: {},
        MyAlias: "BaseObj",
        MyObj: { base: "MyAlias" },
    };

    const ast = {
        BaseObj: {
            type: "object",
            name: "BaseObj",
            functions: [],
            fields: [],
            base: undefined,
        },
        MyAlias: {
            type: "alias",
            name: "MyAlias",
            aliasedType: "BaseObj",
        },
        MyObj: {
            type: "object",
            name: "MyObj",
            functions: [],
            fields: [],
            base: "MyAlias",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("missing base", () => {
    const data = {
        MyObj: { base: "BaseObj" },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: the base type 'BaseObj' of object 'MyObj' is not an existing type."
    );
});

test("invalid base (native)", () => {
    const data = {
        MyObj: { base: "int64" },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: the base type 'int64' of object 'MyObj' must be of type 'object' (is of type 'native')."
    );
});

test("invalid base (aliased array)", () => {
    const data = {
        MyArr: ["int64"],
        MyAlias: "MyArr",
        MyObj: { base: "MyAlias" },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        "Analyzer: the base type 'MyAlias (aka MyArr)' of object 'MyObj' must be of type 'object' (is of type 'array')."
    );
});
