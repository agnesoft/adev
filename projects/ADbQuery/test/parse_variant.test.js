import * as parser from "../src/parser.js";

test("valid variant", () => {
    const data = {
        Operator: ["And", "Or"],
    };

    const ast = {
        Operator: {
            type: "variant",
            name: "Operator",
            variants: ["And", "Or"],
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("invalid variant type", () => {
    const data = {
        MyVar: [{}, []],
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        "Parser: invalid variant type ('object', must be 'string') of variant 'MyVar'."
    );
});
