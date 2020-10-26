import { jsType } from "./parser_common.js";

function validateVariants(token, variants) {
    for (const variant of variants) {
        if (jsType(variant) != "string") {
            throw `Parser: invalid variant type ('${jsType(
                variant
            )}', must be 'string') of variant '${token}'.`;
        }
    }
}

function variants(token, schema) {
    validateVariants(token, schema[token]);
    return schema[token];
}

export function variantAST(token, schema) {
    return {
        type: "variant",
        name: token,
        variants: variants(token, schema),
    };
}
