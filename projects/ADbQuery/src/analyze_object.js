import { realType, typeExists, typeType } from "./analyzer_common.js";

function baseTypeName(baseType, node) {
    if (node["base"] == baseType) {
        return baseType;
    } else {
        return `${node["base"]} (aka ${baseType})`;
    }
}

function validateBaseTypeExists(baseType, node) {
    if (!baseType) {
        throw `Analyzer: the base type '${baseTypeName(
            baseType,
            node
        )}' of object '${node["name"]}' is not an existing type.`;
    }
}

function validateBaseTypeIsObject(baseType, node) {
    const type = typeType(baseType);

    if (type != "object") {
        throw `Analyzer: the base '${baseTypeName(
            baseType,
            node
        )}' of object '${
            node["name"]
        }' must be of type 'object' (is of type '${type}').`;
    }
}

function validateBaseType(baseType, node) {
    validateBaseTypeExists(baseType, node);
    validateBaseTypeIsObject(baseType, node);
}

function validateBase(node, ast) {
    if (node["base"]) {
        validateBaseType(realType(node["base"], ast), ast);
    }
}

function validateFieldTypeExists(field, node, ast) {
    if (!typeExists(field, ast)) {
        throw `Analyzer: the field '${field}' of object '${node["name"]}' is not an existing type.`;
    }
}

function validateFieldType(field, node, ast) {
    const type = typeType(field);

    if (type == "function") {
        throw `Analyzer: the field '${field}' of object '${node["name"]}' cannot be a function.`;
    }
}

function validateField(field, node, ast) {
    validateFieldTypeExists(field, node, ast);
    validateFieldType(field, node, ast);
}

function validateFields(node, ast) {
    for (const field of node["fields"]) {
        validateField(field, node, ast);
    }
}

export function analyzeObject(node, ast) {
    validateBase(node, ast);
    validateFields(node, ast);
}
