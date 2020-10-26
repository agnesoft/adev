import { expressionAST } from "./parse_expression.js";
import { jsType } from "./parser_common.js";

function validateArgument(name, arg) {
    if (jsType(arg) != "string") {
        throw `Parser: invalid argument type ('${jsType(
            arg
        )}', must be 'string') in function '${name}'.`;
    }
}

function validateArguments(name, args) {
    if (jsType(args) != "array") {
        throw `Parser: invalid 'arguments' type ('${jsType(
            args
        )}', must be 'array') in function '${name}'.`;
    }

    for (const argument of args) {
        validateArgument(name, argument);
    }
}

function functionArguments(name, func) {
    if ("arguments" in func) {
        validateArguments(name, func["arguments"]);
        return func["arguments"];
    }

    return [];
}

function validateExpression(name, expression) {
    if (jsType(expression) != "string") {
        throw `Parser: invalid expression ('${jsType(
            expression
        )}', must be 'string') in function '${name}'.`;
    }
}

function validateBody(name, body) {
    if (jsType(body) != "array") {
        throw `Parser: invalid 'body' type ('${jsType(
            body
        )}', must be 'array') in function '${name}'.`;
    }

    for (const expression of body) {
        validateExpression(name, expression);
    }
}

function functionBody(name, func) {
    let expressions = [];

    if ("body" in func) {
        validateBody(name, func["body"]);

        for (const expression of func["body"]) {
            expressions.push(expressionAST(expression.trim()));
        }
    }

    return expressions;
}

function functionReturn(name, func) {
    if (func["return"] && jsType(func["return"]) != "string") {
        throw `Parser: invalid 'return' type ('${jsType(
            func["return"]
        )}', must be 'string') in function '${name}'.`;
    }

    return func["return"];
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
        arguments: functionArguments(name, func),
        body: functionBody(name, func),
        returnValue: functionReturn(name, func),
    };
}
