import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("empty", () => {
    const data = {
        foo: { body: [] },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [],
            returnValue: undefined,
        },
    };

    expect(analyzer.analyze(parser.parse(data))).toEqual(ast);
});

describe("arguments", () => {
    test("valid", () => {
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

    test("missing type", () => {
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
});

describe("return", () => {
    test("missing type", () => {
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

    test("missing return statement", () => {
        const data = {
            Id: "int64",
            foo: {
                return: "Id",
            },
        };

        const analyze = () => {
            return analyzer.analyze(parser.parse(data));
        };

        expect(analyze).toThrow(
            "Analyzer: missing return statement in function 'foo' that has a return value."
        );
    });

    test("missing return statement (object function)", () => {
        const data = {
            Id: "int64",
            MyObj: {
                functions: {
                    foo: {
                        return: "Id",
                    },
                },
            },
        };

        const analyze = () => {
            return analyzer.analyze(parser.parse(data));
        };

        expect(analyze).toThrow(
            "Analyzer: missing return statement in function 'MyObj::foo' that has a return value."
        );
    });
});
