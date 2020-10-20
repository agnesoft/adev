export function arrayAST(token, schema) {
    return {
        type: "array",
        name: token,
        arrayType: schema[token][0],
    };
}
