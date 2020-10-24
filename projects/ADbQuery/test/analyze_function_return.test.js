import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("return new", () => {
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
            body: [
                {
                    type: "return",
                    value: "Id",
                    returnType: "new",
                },
            ],
            returnValue: "Id",
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("return field", () => {
    const data = {
        Id: "int64",
        MyObj: {
            fields: ["Id"],
            functions: {
                foo: {
                    return: "Id",
                },
            },
        },
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
        MyObj: {
            type: "object",
            name: "MyObj",
            base: undefined,
            fields: ["Id"],
            functions: [
                {
                    type: "function",
                    name: "foo",
                    arguments: [],
                    returnValue: "Id",
                    body: [
                        {
                            type: "return",
                            value: "Id",
                            returnType: "field",
                        },
                    ],
                },
            ],
        },
    };

    const analyze = () => {
        return analyzer.analyze(parser.parse(data));
    };

    expect(analyze()).toEqual(ast);
});

test("return argument", () => {
    const data = {
        Id: "int64",
        MyObj: {
            functions: {
                foo: {
                    arguments: ["Id"],
                    return: "Id",
                },
            },
        },
    };

    const ast = {
        Id: {
            type: "alias",
            name: "Id",
            aliasedType: "int64",
        },
        MyObj: {
            type: "object",
            name: "MyObj",
            base: undefined,
            fields: [],
            functions: [
                {
                    type: "function",
                    name: "foo",
                    arguments: ["Id"],
                    returnValue: "Id",
                    body: [
                        {
                            type: "return",
                            value: "Id",
                            returnType: "argument",
                        },
                    ],
                },
            ],
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
