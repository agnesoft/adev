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

describe("invalid body", () => {
    test("wrong body type", () => {
        const data = {
            foo: { body: {} },
        };

        const parse = () => {
            parser.parse(data);
        };

        expect(parse).toThrow(
            "Parser: invalid 'body' type ('object', must be 'array') in function 'foo'."
        );
    });

    test("invalid expression type", () => {
        const data = {
            foo: { body: [{}] },
        };

        const parse = () => {
            parser.parse(data);
        };

        expect(parse).toThrow(
            "Parser: invalid expression ('object', must be 'string') in function 'foo'."
        );
    });
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

    test("invalid arguments", () => {
        const data = {
            foo: { arguments: {} },
        };

        const parse = () => {
            parser.parse(data);
        };

        expect(parse).toThrow(
            "Parser: invalid 'arguments' type ('object', must be 'array') in function 'foo'."
        );
    });

    test("invalid argument", () => {
        const data = {
            foo: { arguments: [{}] },
        };

        const parse = () => {
            parser.parse(data);
        };

        expect(parse).toThrow(
            "Parser: invalid argument type ('object', must be 'string') in function 'foo'."
        );
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

    test("invalid", () => {
        const data = {
            foo: { return: {} },
        };

        const parse = () => {
            parser.parse(data);
        };

        expect(parse).toThrow(
            "Parser: invalid 'return' type ('object', must be 'string') in function 'foo'."
        );
    });
});
