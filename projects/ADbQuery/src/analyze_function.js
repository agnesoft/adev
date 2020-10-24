import { typeExists } from "./analyzer_common.js";

function functionName(node, object) {
    if (object) {
        return `${object["name"]}::${node["name"]}`;
    }

    return `${node["name"]}`;
}

function validateArguments(node, object, ast) {
    for (const arg of node["arguments"]) {
        if (!typeExists(arg, ast)) {
            throw `Analyzer: the argument '${arg}' in function '${functionName(
                node,
                object
            )}' is not an existing type.`;
        }
    }
}

function hasReturnStatement(node) {
    for (const expression of node["body"]) {
        if (expression["type"] == "return") {
            return true;
        }
    }

    return false;
}

function validateReturn(node, object, ast) {
    if (node["returnValue"]) {
        if (!typeExists(node["returnValue"], ast)) {
            throw `Analyzer: the return type '${
                node["returnValue"]
            }' of function '${functionName(
                node,
                object
            )}' is not an existing type.`;
        }

        if (!hasReturnStatement(node)) {
            throw `Analyzer: missing return statement in function '${functionName(
                node,
                object
            )}' that has a return value.`;
        }
    }
}

function isArgument(type, node) {
    return node["arguments"].includes(type);
}

function isField(type, context) {
    return context["type"] == "object" && context["fields"].includes(type);
}

function isArrayType(type, context) {
    return context["type"] == "array" && context["arrayType"] == type;
}

function isVariant(type, context) {
    return context["type"] == "variant" && context["variants"].includes(type);
}

function expressionType(expression, node, context) {
    if (context) {
        if (isField(expression["value"], context)) {
            return "field";
        }

        if (isArrayType(expression["value"], context)) {
            return "arrayType";
        }

        if (isVariant(expression["value"], context)) {
            return "variant";
        }
    }

    if (!expression["parent"] && isArgument(expression["value"], node)) {
        return "argument";
    }

    return "new";
}

function setExpressionType(expression, node, context) {
    if (expression["type"] == "return") {
        expression["returnType"] = expressionType(expression, node, context);
    } else if (expression["type"] == "type") {
        expression["type"] = expressionType(expression, node, context);
    }
}

function expressionContext(expression, object, ast) {
    const parent = expression["parent"];

    if (parent) {
        return ast[parent["value"]];
    }

    return object;
}

function analyzeExpression(expression, node, object, ast) {
    const context = expressionContext(expression, object, ast);

    if (expression["parent"]) {
        analyzeExpression(expression["parent"], node, object, ast);
    }

    setExpressionType(expression, node, context);
}

function analyzeExpressions(node, object, ast) {
    for (let expression of node["body"]) {
        analyzeExpression(expression, node, object, ast);
    }
}

export function analyzeFunction(node, object, ast) {
    validateArguments(node, object, ast);
    validateReturn(node, object, ast);
    analyzeExpressions(node, object, ast);
    return ast;
}
