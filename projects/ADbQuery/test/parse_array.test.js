import * as parser from "../src/parser.js";

test("valid", () => {
    const data = {
        String: ["byte"],
    };

    const ast = {
        String: {
            type: "array",
            name: "String",
            arrayType: "byte",
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});

test("invalid", () => {
    const data = {
        MyArr: [{}],
    };

    const parse = () => {
        parser.parse(data);
    };

    expect(parse).toThrow(
        `Parser: invalid array type ('object', must be 'string') in array 'MyArr'.`
    );
});
