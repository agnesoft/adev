import { analyzeAlias } from "./analyze_alias.js";
import { analyzeArray } from "./analyze_array.js";
import { analyzeFunction } from "./analyze_function";
import { analyzeObject } from "./analyze_object";
import { analyzeVariant } from "./analyze_variant.js";

function analyzeType(node, ast) {
    switch (node["type"]) {
        case "alias":
            analyzeAlias(node, ast);
            break;
        case "array":
            analyzeArray(node, ast);
            break;
        case "variant":
            analyzeVariant(node, ast);
            break;
        case "object":
            analyzeObject(node, ast);
            break;
        case "function":
            analyzeFunction(node, ast);
            break;
        default:
            throw `Analyzer: unknown type '${node["type"]}' named '${node["name"]}'.`;
    }
}

export function analyze(ast) {
    for (const type in ast) {
        analyzeType(ast[type], ast);
    }

    return ast;
}
