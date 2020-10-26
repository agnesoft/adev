import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

test("unknown ast type", () => {
    const ast = {
        foo: {
            type: "function",
            name: "foo",
            arguments: [],
            returnValue: undefined,
            body: [
                {
                    type: "unknown_type",
                },
            ],
        },
    };

    const analyze = () => {
        return analyzer.analyze(ast);
    };

    expect(analyze).toThrow(
        "Analyzer: invalid expression in function 'foo'. Unknown expression type 'unknown_type'."
    );
});

test("incompatible expresion parts", () => {
    const data = {
        Id: "int64",
        MyVar: ["int64", "byte"],
        foo: {
            arguments: ["MyVar"],
            body: ["MyVar.Id = 1"],
        },
    };

    const analyze = () => {
        analyzer.analyze(parser.parse(data));
    };

    expect(analyze).toThrow(
        `Analyzer: invalid expression in function 'foo'. The 'Id' cannot be accessed via 'MyVar'.`
    );
});

describe("assignment", () => {
    test("argument to field", () => {
        const data = {
            Id: "int64",
            From: "Id",
            Obj: {
                fields: ["From"],
                functions: {
                    foo: {
                        arguments: ["Id"],
                        body: ["From = Id"],
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
            From: {
                type: "alias",
                name: "From",
                aliasedType: "Id",
            },
            Obj: {
                type: "object",
                name: "Obj",
                fields: ["From"],
                base: undefined,
                functions: [
                    {
                        type: "function",
                        name: "foo",
                        arguments: ["Id"],
                        returnValue: undefined,
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "field",
                                    value: "From",
                                },
                                right: {
                                    type: "argument",
                                    value: "Id",
                                },
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

    test("nested types", () => {
        const data = {
            Id: "int64",
            ArgObj: { fields: ["Id"] },
            BaseObj: { fields: ["Id"] },
            DataObj: { base: "BaseObj" },
            Obj: {
                fields: ["DataObj"],
                functions: {
                    foo: {
                        arguments: ["ArgObj"],
                        body: ["DataObj.Id = ArgObj.Id"],
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
            ArgObj: {
                type: "object",
                name: "ArgObj",
                fields: ["Id"],
                base: undefined,
                functions: [],
            },
            BaseObj: {
                type: "object",
                name: "BaseObj",
                fields: ["Id"],
                base: undefined,
                functions: [],
            },
            DataObj: {
                type: "object",
                name: "DataObj",
                fields: [],
                base: "BaseObj",
                functions: [],
            },
            Obj: {
                type: "object",
                name: "Obj",
                fields: ["DataObj"],
                base: undefined,
                functions: [
                    {
                        type: "function",
                        name: "foo",
                        arguments: ["ArgObj"],
                        returnValue: undefined,
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "field",
                                    value: "Id",
                                    parent: {
                                        type: "field",
                                        value: "DataObj",
                                    },
                                },
                                right: {
                                    type: "field",
                                    value: "Id",
                                    parent: {
                                        type: "argument",
                                        value: "ArgObj",
                                    },
                                },
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
});

describe("return", () => {
    test("new", () => {
        const data = {
            Id: "int64",
            foo: {
                body: ["return Id"],
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

    test("field", () => {
        const data = {
            Id: "int64",
            MyObj: {
                fields: ["Id"],
                functions: {
                    foo: {
                        body: ["return Id"],
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

    test("parent field", () => {
        const data = {
            Id: "int64",
            BaseObj: {
                fields: ["Id"],
            },
            MyObj: {
                base: "BaseObj",
                functions: {
                    foo: {
                        body: ["return Id"],
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
            BaseObj: {
                type: "object",
                name: "BaseObj",
                base: undefined,
                fields: ["Id"],
                functions: [],
            },
            MyObj: {
                type: "object",
                name: "MyObj",
                base: "BaseObj",
                fields: [],
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

    test("field of field", () => {
        const data = {
            Id: "int64",
            SubObj: {
                fields: ["Id"],
            },
            MyObj: {
                fields: ["SubObj"],
                functions: {
                    foo: {
                        body: ["return SubObj.Id"],
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
            SubObj: {
                type: "object",
                name: "SubObj",
                base: undefined,
                fields: ["Id"],
                functions: [],
            },
            MyObj: {
                type: "object",
                name: "MyObj",
                base: undefined,
                fields: ["SubObj"],
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
                                parent: {
                                    type: "field",
                                    value: "SubObj",
                                },
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

    test("field of field from variant field", () => {
        const data = {
            Id: "int64",
            SubObj: {
                fields: ["Id"],
            },
            MyVar: ["Id", "SubObj"],
            MyObj: {
                fields: ["MyVar"],
                functions: {
                    foo: {
                        body: ["MyVar.SubObj.Id = 1", "return MyVar.SubObj.Id"],
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
            SubObj: {
                type: "object",
                name: "SubObj",
                base: undefined,
                fields: ["Id"],
                functions: [],
            },
            MyVar: {
                type: "variant",
                name: "MyVar",
                variants: ["Id", "SubObj"],
            },
            MyObj: {
                type: "object",
                name: "MyObj",
                base: undefined,
                fields: ["MyVar"],
                functions: [
                    {
                        type: "function",
                        name: "foo",
                        arguments: [],
                        returnValue: "Id",
                        body: [
                            {
                                type: "assignment",
                                left: {
                                    type: "field",
                                    value: "Id",
                                    parent: {
                                        type: "variant",
                                        value: "SubObj",
                                        parent: {
                                            type: "field",
                                            value: "MyVar",
                                        },
                                    },
                                },
                                right: {
                                    type: "number",
                                    value: 1,
                                },
                            },
                            {
                                type: "return",
                                value: "Id",
                                returnType: "field",
                                parent: {
                                    type: "variant",
                                    value: "SubObj",
                                    parent: {
                                        type: "field",
                                        value: "MyVar",
                                    },
                                },
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

    test("array type from variant field", () => {
        const data = {
            Id: "int64",
            MyArr: ["Id"],
            MyVar: ["Id", "MyArr"],
            MyObj: {
                fields: ["MyVar"],
                functions: {
                    foo: {
                        body: ["return MyVar.MyArr.Id"],
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
            MyArr: {
                type: "array",
                name: "MyArr",
                arrayType: "Id",
            },
            MyVar: {
                type: "variant",
                name: "MyVar",
                variants: ["Id", "MyArr"],
            },
            MyObj: {
                type: "object",
                name: "MyObj",
                base: undefined,
                fields: ["MyVar"],
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
                                returnType: "arrayType",
                                parent: {
                                    type: "variant",
                                    value: "MyArr",
                                    parent: {
                                        type: "field",
                                        value: "MyVar",
                                    },
                                },
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

    test("argument", () => {
        const data = {
            Id: "int64",
            MyObj: {
                functions: {
                    foo: {
                        arguments: ["Id"],
                        body: ["return Id"],
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
});
