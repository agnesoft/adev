import * as parser from "../src/parser.js";
import * as analyzer from "../src/analyzer.js";

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
                        body: ["return MyVar.SubObj.Id"],
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
