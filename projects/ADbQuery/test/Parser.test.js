import Parser from "../Parser.js";

describe("Parser(data)", () => {
    test("[data]", () => {
        const constructParser = () => {
            new Parser({});
        };
        expect(constructParser).not.toThrow();
    });
});

describe("parse()", () => {
    test("[unknown type]", () => {
        const data = {
            Value: 10,
        };
        const parser = () => {
            new Parser(data).parse();
        };
        expect(parser).toThrow("Unknown token type: Value [number]");
    });

    test("[alias]", () => {
        const data = {
            Size: "int64",
        };

        const ast = [
            {
                type: "alias",
                name: "Size",
                aliasedType: "int64",
            },
        ];

        expect(new Parser(data).parse()).toEqual(ast);
    });

    test("[array]", () => {
        const data = {
            String: ["byte"],
        };

        const ast = [
            {
                type: "array",
                name: "String",
                arrayType: "byte",
            },
        ];

        expect(new Parser(data).parse()).toEqual(ast);
    });

    test("[variant]", () => {
        const data = {
            Operator: ["And", "Or"],
        };

        const ast = [
            {
                type: "variant",
                name: "Operator",
                variants: ["And", "Or"],
            },
        ];

        expect(new Parser(data).parse()).toEqual(ast);
    });

    describe("[function]", () => {
        test("[return]", () => {
            const data = {
                foo: {
                    return: "String",
                },
            };

            const ast = [
                {
                    type: "function",
                    name: "foo",
                    arguments: [],
                    body: [],
                    returnValue: "String",
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        test("[argument]", () => {
            const data = {
                foo: {
                    arguments: ["Size"],
                },
            };

            const ast = [
                {
                    type: "function",
                    name: "foo",
                    arguments: ["Size"],
                    body: [],
                    returnValue: undefined,
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        test("[arguments]", () => {
            const data = {
                foo: {
                    arguments: ["Size", "String"],
                },
            };

            const ast = [
                {
                    type: "function",
                    name: "foo",
                    arguments: ["Size", "String"],
                    body: [],
                    returnValue: undefined,
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        describe("[body]", () => {
            test("[single expression]", () => {
                const data = {
                    foo: {
                        body: ["String"],
                    },
                };

                const ast = [
                    {
                        type: "function",
                        name: "foo",
                        arguments: [],
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "type",
                                    value: "String",
                                    parent: undefined,
                                },
                                right: undefined,
                            },
                        ],
                        returnValue: undefined,
                    },
                ];

                expect(new Parser(data).parse()).toEqual(ast);
            });

            test("[compound expression]", () => {
                const data = {
                    foo: {
                        body: ["String.Size"],
                    },
                };

                const ast = [
                    {
                        type: "function",
                        name: "foo",
                        arguments: [],
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "type",
                                    value: "Size",
                                    parent: {
                                        type: "type",
                                        value: "String",
                                        parent: undefined,
                                    },
                                },
                                right: undefined,
                            },
                        ],
                        returnValue: undefined,
                    },
                ];

                expect(new Parser(data).parse()).toEqual(ast);
            });

            test("[assignment - number]", () => {
                const data = {
                    foo: {
                        body: ["String.Size = 1"],
                    },
                };

                const ast = [
                    {
                        type: "function",
                        name: "foo",
                        arguments: [],
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "type",
                                    value: "Size",
                                    parent: {
                                        type: "type",
                                        value: "String",
                                        parent: undefined,
                                    },
                                },
                                right: {
                                    type: "number",
                                    value: 1,
                                    parent: undefined,
                                },
                            },
                        ],
                        returnValue: undefined,
                    },
                ];

                expect(new Parser(data).parse()).toEqual(ast);
            });
        });
    });

    describe("[object]", () => {
        test("[empty]", () => {
            const data = {
                Obj: {},
            };

            const ast = [
                {
                    type: "object",
                    name: "Obj",
                    base: undefined,
                    fields: [],
                    functions: [],
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        test("[base]", () => {
            const data = {
                Obj: { base: "BaseObj" },
            };

            const ast = [
                {
                    type: "object",
                    name: "Obj",
                    base: "BaseObj",
                    fields: [],
                    functions: [],
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        test("[fields]", () => {
            const data = {
                Obj: { fields: ["field1", "field2"] },
            };

            const ast = [
                {
                    type: "object",
                    name: "Obj",
                    base: undefined,
                    fields: ["field1", "field2"],
                    functions: [],
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

        test("[methods]", () => {
            const data = {
                Obj: {
                    functions: {
                        foo: {
                            body: ["String", "String.Size = 1"],
                        },
                        bar: {
                            body: ["MyArr = Ids"],
                        },
                    },
                },
            };

            const ast = [
                {
                    type: "object",
                    name: "Obj",
                    base: undefined,
                    fields: [],
                    functions: [
                        {
                            type: "function",
                            name: "foo",
                            arguments: [],
                            body: [
                                {
                                    type: "assignment",
                                    left: {
                                        type: "type",
                                        value: "String",
                                        parent: undefined,
                                    },
                                    right: undefined,
                                },
                                {
                                    type: "assignment",
                                    left: {
                                        type: "type",
                                        value: "Size",
                                        parent: {
                                            type: "type",
                                            value: "String",
                                            parent: undefined,
                                        },
                                    },
                                    right: {
                                        type: "number",
                                        value: 1,
                                        parent: undefined,
                                    },
                                },
                            ],
                        },
                        {
                            type: "function",
                            name: "bar",
                            arguments: [],
                            body: [
                                {
                                    type: "assignment",
                                    left: {
                                        type: "type",
                                        value: "MyArr",
                                        parent: undefined,
                                    },
                                    right: {
                                        type: "type",
                                        value: "Ids",
                                        parent: undefined,
                                    },
                                },
                            ],
                        },
                    ],
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });
    });
});
