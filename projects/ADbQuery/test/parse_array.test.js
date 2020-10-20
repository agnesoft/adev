import { TestScheduler } from "jest";
import * as parser from "../src/parser.js";

test("valid array", () => {
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
