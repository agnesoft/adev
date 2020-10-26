import { aliasAST } from "./parse_alias.js";
import { arrayAST } from "./parse_array.js";
import { objectAST } from "./parse_object.js";
import { isFunction, functionAST } from "./parse_function";
import { variantAST } from "./parse_variant.js";

function tokenType(token, schema) {
    if (Array.isArray(schema[token])) {
        return "array";
    } else {
        return typeof schema[token];
    }
}

function parseArrayToken(token, schema) {
    if (schema[token].length == 1) {
        return arrayAST(token, schema);
    } else {
        return variantAST(token, schema);
    }
}

function parseObjectToken(token, schema) {
    if (isFunction(token, schema)) {
        return functionAST(token, schema[token]);
    } else {
        return objectAST(token, schema);
    }
}

function parseStringToken(token, schema) {
    return aliasAST(token, schema);
}

function parseToken(token, schema) {
    const type = tokenType(token, schema);

    switch (type) {
        case "string":
            return parseStringToken(token, schema);
        case "array":
            return parseArrayToken(token, schema);
        case "object":
            return parseObjectToken(token, schema);
        default:
            throw `Unknown token type '${type}' of token '${token}'.`;
    }
}

export function parse(schema) {
    let ast = {};

    for (const token in schema) {
        ast[token] = parseToken(token, schema);
    }

    return ast;
}
