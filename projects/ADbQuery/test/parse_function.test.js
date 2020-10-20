import * as parser from "../src/parser.js";

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
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

describe("arguments", () => {
    test("single", () => {
        const data = {
            foo: { arguments: ["arg1"] },
        };

        const ast = {
            foo: {
                type: "function",
                name: "foo",
                arguments: ["arg1"],
                body: [],
            },
        };

        expect(parser.parse(data)).toEqual(ast);
    });

    test("multiple arguments", () => {
        const data = {
            foo: { arguments: ["arg1", "arg2"] },
        };

        const ast = {
            foo: {
                type: "function",
                name: "foo",
                arguments: ["arg1", "arg2"],
                body: [],
            },
        };

        expect(parser.parse(data)).toEqual(ast);
    });
});

describe("return value", () => {
    test("valid", () => {
        const data = {
            foo: { return: "Id" },
        };

        const ast = {
            foo: {
                type: "function",
                name: "foo",
                arguments: [],
                body: [],
                returnValue: "Id",
            },
        };

        expect(parser.parse(data)).toEqual(ast);
    });
});
