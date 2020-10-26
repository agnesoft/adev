export default class Analyzer {
    constructor(ast) {
        this._ast = ast;
    }

    analyzeArray(ast) {
        this.verifyType(ast["arrayType"], ast);
    }

    analyzeVariant(ast) {
        for (const variant of ast["variants"]) {
            this.verifyType(variant, ast);
        }
    }

    realType(ast) {
        if (!ast) {
            return undefined;
        }

        const type = ast["type"];

        if (type == "alias") {
            return this.realType(this.findAST(ast["aliasedType"]));
        }

        return type;
    }

    isObject(ast) {
        return this.realType(ast) == "object";
    }

    verifyBaseAST(base, descendant) {
        if (!this.isObject(base)) {
            throw `Analyzer: the base '${base["name"]}' of object '${
                descendant["name"]
            }' is of type '${this.realType(base)}' (must be 'object').`;
        }
    }

    verifyBase(ast) {
        const base = ast["base"];

        if (base) {
            const baseAST = this.findAST(base);

            if (baseAST) {
                this.verifyBaseAST(baseAST, ast);
            } else {
                throw `Analyzer: the base '${base}' of object '${ast["name"]}' is not an existing type.`;
            }
        }
    }

    verifyFields(ast) {
        for (const field of ast["fields"]) {
            if (!this.typeExists(field)) {
                throw `Analyzer: the field '${field}' of object '${ast["name"]}' is not an existing type.`;
            }
        }
    }

    analyzeObject(ast) {
        this.verifyBase(ast);
        this.verifyFields(ast);
    }

    verifyArguments(ast) {
        for (const arg of ast["arguments"]) {
            if (!this.typeExists(arg)) {
                throw `Analyzer: the argument '${arg}' of function '${ast["name"]}' is not an existing type.`;
            }
        }
    }

    verifyReturnValue(ast) {
        if (ast["returnValue"]) {
            if (!this.typeExists(ast["returnValue"])) {
                throw `Analyzer: the return value '${ast["returnValue"]}' of function '${ast["name"]}' is not an existing type.`;
            }
        }
    }

    functionName(func, object) {
        if (object) {
            return `${object["name"]}::${func["name"]}`;
        }

        return func["name"];
    }

    verifyExpressionPart(part, func, object) {}

    verifyReferencedTypes(expression, func, object) {
        this.verifyExpressionPart(expression["left"]);

        if (expression["right"]) {
            this.verifyExpressionPart(expression);
        }

        if (expression["right"]) {
            for (const part of expression["right"]) {
            }
        }
    }

    verifyAssignmentTypes(expression, func, object) {
        if (!expression["right"]) {
            if (expression["left"]["type"] == "number") {
                throw `Analyzer: declaring a number does not make sense (in function '${this.functionName(
                    func,
                    object
                )}').`;
            }

            return;
        }

        if (expression["left"]["type"] == "number") {
            if (expression["right"]["type"] == "number") {
                throw `Analyzer: assigning a number to a number does not make sense (in function '${this.functionName(
                    func,
                    object
                )}')`;
            }
        }
    }

    analyzeAssignment(expression, func, object) {
        this.verifyReferencedTypes(expression, func, object);
        this.verifyAssignmentTypes(expression, func, object);
    }

    analyzeExpression(expression, func, object) {
        switch (expression["type"]) {
            case "assignment":
                this.analyzeAssignment(expression, func, object);
                break;
            default:
                throw `Analyzer: unknown expression type '${
                    expression["type"]
                }' in function '${this.functionName(func, object)}'.`;
        }
    }

    analyzeBody(ast, obj) {
        for (const expression of ast["body"]) {
            this.analyzeExpression(expression, ast, obj);
        }
    }

    analyzeFunction(ast, object) {
        this.verifyArguments(ast);
        this.verifyReturnValue(ast);

        if ("body" in ast) {
            this.analyzeBody(ast, object);
        }
    }
}
