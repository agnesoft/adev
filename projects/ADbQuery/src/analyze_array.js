import { typeExists } from "./analyzer_common.js";

export function analyzeArray(node, ast) {
    if (!typeExists(node["arrayType"], ast)) {
        throw `Analyzer: unknown type '${node["arrayType"]}' used as an array type of '${node["name"]}'.`;
    }

    return node;
}
