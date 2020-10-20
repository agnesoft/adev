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
