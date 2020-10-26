import { jsType } from "./parser_common.js";

function validateArrayType(token, type) {
    if (jsType(type) != "string") {
        throw `Parser: invalid array type ('${jsType(
            type
        )}', must be 'string') in array '${token}'.`;
    }
}

function arrayType(token, schema) {
    validateArrayType(token, schema[token][0]);
    return schema[token][0];
}

export function arrayAST(token, schema) {
    return {
        type: "array",
        name: token,
        arrayType: arrayType(token, schema),
    };
}
