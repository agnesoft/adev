import { typeExists } from "./analyzer_common.js";

function isArgument(type, node) {
    return node["arguments"].includes(type);
}

function isParentField(type, context, ast) {
    return context["base"] && isField(type, ast[context["base"]], ast);
}

function isField(type, context, ast) {
    return (
        context["type"] == "object" &&
        (context["fields"].includes(type) || isParentField(type, context, ast))
    );
}

function isArrayType(type, context) {
    return context["type"] == "array" && context["arrayType"] == type;
}

function isVariant(type, context) {
    return context["type"] == "variant" && context["variants"].includes(type);
}

function expressionType(expression, node, context, ast) {
    if (context) {
        if (isField(expression["value"], context, ast)) {
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

    if (!isNaN(expression["value"])) {
        return "number";
    }

    return "new";
}

function expressionContext(expression, object, ast) {
    const parent = expression["parent"];

    if (parent) {
        return ast[parent["value"]];
    }

    return object;
}

function analyzeExpressionPart(expression, node, object, ast) {
    const context = expressionContext(expression, object, ast);

    if (expression["parent"]) {
        analyzeExpressionPart(expression["parent"], node, object, ast);
    }

    expression["type"] = expressionType(expression, node, context, ast);
}

function analyzeReturn(expression, node, object, ast) {
    const context = expressionContext(expression, object, ast);

    if (expression["parent"]) {
        analyzeExpressionPart(expression["parent"], node, object, ast);
    }

    expression["returnType"] = expressionType(expression, node, context, ast);
}

function analyzeAssignment(expression, node, object, ast) {
    analyzeExpressionPart(expression["left"], node, object, ast);
    analyzeExpressionPart(expression["right"], node, object, ast);
}

export function analyzeExpression(expression, node, object, ast) {
    switch (expression["type"]) {
        case "return":
            analyzeReturn(expression, node, object, ast);
            break;
        case "assignment":
            analyzeAssignment(expression, node, object, ast);
            break;
        default:
            throw `Analyzer: unknown expressiong type '${expression["type"]}'.`;
    }
}
