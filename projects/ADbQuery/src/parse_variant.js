export function variantAST(token, schema) {
    return {
        type: "variant",
        name: token,
        variants: schema[token],
    };
}
