import { typeExists } from "./analyzer_common.js";

export function analyzeAlias(node, ast) {
    if (!typeExists(node["aliasedType"], ast)) {
        throw `Analyzer: unknown type '${node["aliasedType"]}' aliased as '${node["name"]}'.`;
    }

    return node;
}
