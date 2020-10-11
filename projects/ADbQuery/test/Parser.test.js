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

    test("[union]", () => {
        const data = {
            Operator: ["And", "Or"],
        };

        const ast = [
            {
                type: "union",
                name: "Operator",
                variants: ["And", "Or"],
            },
        ];

        expect(new Parser(data).parse()).toEqual(ast);
    });

    describe("[function]", () => {
        test("[empty]", () => {
            const data = {
                foo: {},
            };

            const ast = [
                {
                    type: "function",
                    name: "foo",
                    arguments: [],
                    body: [],
                    returnValue: undefined,
                },
            ];

            expect(new Parser(data).parse()).toEqual(ast);
        });

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
});
