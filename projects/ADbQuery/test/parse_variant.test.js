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
