import { functionAST } from "./parse_function.js";
import { jsType } from "./parser_common.js";

function validateField(token, field) {
    if (jsType(field) != "string") {
        throw `Parser: invalid field type ('${jsType(
            field
        )}', must be 'string') in object '${token}'.`;
    }
}

function validateFields(token, fields) {
    if (jsType(fields) != "array") {
        throw `Parser: invalid 'fields' type ('${jsType(
            fields
        )}', must be 'array') in object '${token}'.`;
    }

    for (const field of fields) {
        validateField(token, field);
    }
}

function objectFields(token, schema) {
    if ("fields" in schema[token]) {
        validateFields(token, schema[token]["fields"]);
        return schema[token]["fields"];
    }

    return [];
}

function validateFunction(token, func, definition) {
    if (jsType(definition) != "object") {
        throw `Parser: invalid function type ('${jsType(
            definition
        )}', must be 'object') in function '${token}::${func}'.`;
    }
}

function validateFunctions(token, functions) {
    if (jsType(functions) != "object") {
        throw `Parser: invalid 'functions' type ('${jsType(
            functions
        )}', must be 'object') in object '${token}'.`;
    }

    for (const func in functions) {
        validateFunction(token, func, functions[func]);
    }
}

function objectFunctions(token, schema) {
    let functions = [];

    if ("functions" in schema[token]) {
        validateFunctions(token, schema[token]["functions"]);

        for (const func in schema[token]["functions"]) {
            functions.push(functionAST(func, schema[token]["functions"][func]));
        }
    }

    return functions;
}

function objectBase(token, schema) {
    if ("base" in schema[token] && typeof schema[token]["base"] != "string") {
        throw `Parser: invalid 'base' type ('${typeof schema[token][
            "base"
        ]}', must be 'string') in object '${token}'.`;
    }

    return schema[token]["base"];
}

export function objectAST(token, schema) {
    return {
        type: "object",
        name: token,
        base: objectBase(token, schema),
        fields: objectFields(token, schema),
        functions: objectFunctions(token, schema),
    };
}
