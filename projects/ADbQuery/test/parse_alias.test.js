import * as parser from "../src/parser.js";

test("valiad alias", () => {
    const data = {
        Size: "int64",
    };

    const ast = {
        Size: {
            type: "alias",
            name: "Size",
            aliasedType: "int64",
        },
    };

    expect(parser.parse(data)).toEqual(ast);
});
