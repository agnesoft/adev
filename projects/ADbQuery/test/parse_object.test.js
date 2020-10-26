import * as parser from "../src/parser.js";

test("empty", () => {
    const data = {
        MyObj: {},
    };

    const ast = {
        MyObj: {
            type: "object",
            name: "MyObj",
            fields: [],
            functions: [],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("base", () => {
    const data = {
        MyObj: {
            base: "SomeObj",
        },
    };

    const ast = {
        MyObj: {
            type: "object",
            name: "MyObj",
            fields: [],
            functions: [],
            base: "SomeObj",
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("fields", () => {
    const data = {
        MyObj: {
            fields: ["field1", "field2"],
        },
    };

    const ast = {
        MyObj: {
            type: "object",
            name: "MyObj",
            fields: ["field1", "field2"],
            functions: [],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("functions", () => {
    const data = {
        MyObj: {
            fields: ["Id"],
            functions: {
                foo: {
                    arguments: ["arg1"],
                    body: ["Id = arg1"],
                },
                bar: {
                    return: "Id",
                },
            },
        },
    };

    const ast = {
        MyObj: {
            type: "object",
            name: "MyObj",
            base: undefined,
            fields: ["Id"],
            functions: [
                {
                    type: "function",
                    name: "foo",
                    arguments: ["arg1"],
                    body: [
                        {
                            type: "assignment",
                            left: {
                                type: "type",
                                value: "Id",
                            },
                            right: {
                                type: "type",
                                value: "arg1",
                            },
                        },
                    ],
                },
                {
                    type: "function",
                    name: "bar",
                    arguments: [],
                    body: [],
                    returnValue: "Id",
                },
            ],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("invalid base type", () => {
    const data = {
        MyObj: { base: {} },
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid 'base' type ('object', must be 'string') in object 'MyObj'."
    );
});

test("invalid fields type", () => {
    const data = {
        MyObj: { fields: {} },
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid 'fields' type ('object', must be 'array') in object 'MyObj'."
    );
});

test("invalid field type", () => {
    const data = {
        MyObj: { fields: [{}] },
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid field type ('object', must be 'string') in object 'MyObj'."
    );
});

test("invalid functions type", () => {
    const data = {
        MyObj: { functions: [] },
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid 'functions' type ('array', must be 'object') in object 'MyObj'."
    );
});

test("invalid function type", () => {
    const data = {
        MyObj: { functions: { foo: [] } },
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid function type ('array', must be 'object') in function 'MyObj::foo'."
    );
});
