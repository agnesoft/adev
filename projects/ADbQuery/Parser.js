export default class Parser {
    constructor(data) {
        this._data = data;
    }

    tokenType(token) {
        if (Array.isArray(this._data[token])) {
            return "array";
        } else {
            return typeof this._data[token];
        }
    }

    aliasAST(token) {
        return {
            type: "alias",
            name: token,
            aliasedType: this._data[token],
        };
    }

    parseStringToken(token) {
        return this.aliasAST(token);
    }

    arrayAST(token) {
        return {
            type: "array",
            name: token,
            arrayType: this._data[token][0],
        };
    }

    variantAST(token) {
        return {
            type: "variant",
            name: token,
            variants: this._data[token],
        };
    }

    parseArrayToken(token) {
        if (this._data[token].length == 1) {
            return this.arrayAST(token);
        } else {
            return this.variantAST(token);
        }
    }

    isFunctionToken(token) {
        const keys = Object.keys(this._data[token]);
        const test = (element) => keys.includes(element);
        return ["arguments", "body", "return"].some(test);
    }

    functionArguments(func) {
        if ("arguments" in func) {
            return func["arguments"];
        }

        return [];
    }

    expressionSideAST(side) {
        const parts = side.split(".");
        let ast = {};
        parts.reduceRight((part, current) => {
            part["parent"] = {
                type: isNaN(current.trim()) ? "type" : "number",
                value: isNaN(current.trim())
                    ? current.trim()
                    : Number(current.trim()),
                parent: undefined,
            };
            return part["parent"];
        }, ast);
        return ast["parent"];
    }

    expressionAST(expression) {
        const sides = expression.split("=");
        let ast = {
            type: "assignment",
            left: this.expressionSideAST(sides[0].trim()),
            right:
                sides.length > 1
                    ? this.expressionSideAST(sides[1].trim())
                    : undefined,
        };
        return ast;
    }

    functionBody(func) {
        let expressions = [];

        if ("body" in func) {
            for (const expression of func["body"]) {
                expressions.push(this.expressionAST(expression));
            }
        }

        return expressions;
    }

    functionAST(name, func) {
        return {
            type: "function",
            name: name,
            arguments: this.functionArguments(func),
            body: this.functionBody(func),
            returnValue: func["return"],
        };
    }

    objectFields(token) {
        if ("fields" in this._data[token]) {
            return this._data[token]["fields"];
        }

        return [];
    }

    objectFunctions(token) {
        let functions = [];

        if ("functions" in this._data[token]) {
            for (const func in this._data[token]["functions"]) {
                functions.push(
                    this.functionAST(func, this._data[token]["functions"][func])
                );
            }
        }

        return functions;
    }

    objectAST(token) {
        return {
            type: "object",
            name: token,
            base: this._data[token]["base"],
            fields: this.objectFields(token),
            functions: this.objectFunctions(token),
        };
    }

    parseObjectToken(token) {
        if (this.isFunctionToken(token)) {
            return this.functionAST(token, this._data[token]);
        } else {
            return this.objectAST(token);
        }
    }

    parseToken(token) {
        const type = this.tokenType(token);
        switch (type) {
            case "string":
                return this.parseStringToken(token);
            case "array":
                return this.parseArrayToken(token);
            case "object":
                return this.parseObjectToken(token);
            default:
                throw `Unknown token type: ${token} [${type}]`;
        }
    }

    parse() {
        let ast = [];

        for (const token in this._data) {
            ast.push(this.parseToken(token));
        }

        return ast;
    }
}
