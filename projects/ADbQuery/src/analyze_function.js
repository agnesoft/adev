import { typeExists } from "./analyzer_common.js";

function analyzeArguments(node, ast) {
    for (const arg of node["arguments"]) {
        if (!typeExists(arg, ast)) {
            throw `Analyzer: the argument '${arg}' in function '${node["name"]}' is not an existing type.`;
        }
    }
}

function isArgument(type, node) {
    return node["arguments"].includes(type);
}

function isField(type, object) {
    return object && object["fields"].includes(type);
}

function returnType(node, object) {
    if (isArgument(node["returnValue"], node)) {
        return "argument";
    }

    if (isField(node["returnValue"], object)) {
        return "field";
    }

    return "new";
}

function createReturnAST(node, object) {
    return {
        type: "return",
        value: node["returnValue"],
        returnType: returnType(node, object),
    };
}

function analyzeReturn(node, object, ast) {
    if (node["returnValue"]) {
        if (!typeExists(node["returnValue"], ast)) {
            throw `Analyzer: the return type '${node["returnValue"]}' of function '${node["name"]}' is not an existing type.`;
        }

        node["body"].push(createReturnAST(node, object));
    }
}

export function analyzeFunction(node, object, ast) {
    analyzeArguments(node, ast);
    analyzeReturn(node, object, ast);
    return ast;
}
