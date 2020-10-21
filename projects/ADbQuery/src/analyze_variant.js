import { typeExists } from "./analyzer_common.js";

export function analyzeVariant(node, ast) {
    for (const variant of node["variants"]) {
        if (!typeExists(variant, ast)) {
            throw `Analyzer: unknown type '${variant}' used as a variant of '${node["name"]}'.`;
        }
    }

    return node;
}
