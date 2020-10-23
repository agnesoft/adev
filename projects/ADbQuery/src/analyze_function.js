import { typeExists } from "./analyzer_common.js";

function analyzeArguments(node, ast) {
    for (const arg of node["arguments"]) {
        if (!typeExists(arg, ast)) {
            throw `Analyzer: the argument '${arg}' in function '${node["name"]}' is not an existing type.`;
        }
    }
}

function analyzeReturn(node, ast) {
    if (node["returnValue"] && !typeExists(node["returnValue"], ast)) {
        throw `Analyzer: the return type '${node["returnValue"]}' of function '${node["name"]}' is not an existing type.`;
    }
}

export function analyzeFunction(node, object, ast) {
    analyzeArguments(node, ast);
    analyzeReturn(node, ast);
    return ast;
}
