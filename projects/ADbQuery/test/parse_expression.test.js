import * as parser from "../src/parser.js";

test("number to type", () => {
    const data = {
        foo: { body: ["Id = 1"] },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "assignment",
                    left: {
                        type: "type",
                        value: "Id",
                    },
                    right: {
                        type: "number",
                        value: 1,
                    },
                },
            ],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("type to type", () => {
    const data = {
        foo: { body: ["Id = FromId"] },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "assignment",
                    left: {
                        type: "type",
                        value: "Id",
                    },
                    right: {
                        type: "type",
                        value: "FromId",
                    },
                },
            ],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("nested types", () => {
    const data = {
        foo: { body: ["Obj.Id = MyArr.int64"] },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "assignment",
                    left: {
                        type: "type",
                        value: "Id",
                        parent: {
                            type: "type",
                            value: "Obj",
                        },
                    },
                    right: {
                        type: "type",
                        value: "int64",
                        parent: {
                            type: "type",
                            value: "MyArr",
                        },
                    },
                },
            ],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("multiple expressions", () => {
    const data = {
        foo: { body: ["Obj.Id = MyArr.int64", "MyVar.Obj.Field1 = 1"] },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "assignment",
                    left: {
                        type: "type",
                        value: "Id",
                        parent: {
                            type: "type",
                            value: "Obj",
                        },
                    },
                    right: {
                        type: "type",
                        value: "int64",
                        parent: {
                            type: "type",
                            value: "MyArr",
                        },
                    },
                },
                {
                    type: "assignment",
                    left: {
                        type: "type",
                        value: "Field1",
                        parent: {
                            type: "type",
                            value: "Obj",
                            parent: {
                                type: "type",
                                value: "MyVar",
                            },
                        },
                    },
                    right: {
                        type: "number",
                        value: 1,
                    },
                },
            ],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("return type", () => {
    const data = {
        foo: {
            body: ["return Id"],
            return: "Id",
        },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "return",
                    value: "Id",
                    returnType: "type",
                },
            ],
            returnValue: "Id",
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("return nested type", () => {
    const data = {
        foo: {
            body: ["return MyObj.Id"],
            return: "Id",
        },
    };

    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            body: [
                {
                    type: "return",
                    value: "Id",
                    returnType: "type",
                    parent: {
                        type: "type",
                        value: "MyObj",
                    },
                },
            ],
            returnValue: "Id",
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});
