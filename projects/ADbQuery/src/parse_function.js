import { expressionAST } from "./parse_expression.js";

function functionArguments(func) {
    if ("arguments" in func) {
        return func["arguments"];
    }

    return [];
}

function functionBody(func) {
    let expressions = [];

    if ("body" in func) {
        for (const expression of func["body"]) {
            expressions.push(expressionAST(expression));
        }
    }

    return expressions;
}

export function isFunction(token, schema) {
    const keys = Object.keys(schema[token]);
    const test = (element) => keys.includes(element);
    return ["arguments", "body", "return"].some(test);
}

export function functionAST(name, func) {
    return {
        type: "function",
        name: name,
        arguments: functionArguments(func),
        body: functionBody(func),
        returnValue: func["return"],
    };
}
