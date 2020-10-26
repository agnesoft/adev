export function aliasAST(token, schema) {
    return {
        type: "alias",
        name: token,
        aliasedType: schema[token],
    };
}
