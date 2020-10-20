import { functionAST } from "./parse_function.js";

function objectFields(token, schema) {
    if ("fields" in schema[token]) {
        return schema[token]["fields"];
    }

    return [];
}

function objectFunctions(token, schema) {
    let functions = [];

    if ("functions" in schema[token]) {
        for (const func in schema[token]["functions"]) {
            functions.push(functionAST(func, schema[token]["functions"][func]));
        }
    }

    return functions;
}

export function objectAST(token, schema) {
    return {
        type: "object",
        name: token,
        base: schema[token]["base"],
        fields: objectFields(token, schema),
        functions: objectFunctions(token, schema),
    };
}
