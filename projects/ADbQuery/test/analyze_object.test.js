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

describe("base", () => {
    test("valid", () => {
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

    test("aliased", () => {
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

    test("missing", () => {
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

    test("invalid", () => {
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

    test("invalid aliased", () => {
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
});

describe("fields", () => {
    test("valid", () => {
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

    test("missing type", () => {
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
});
