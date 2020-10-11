import { print } from "./common.js";

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

    unionAST(token) {
        return {
            type: "union",
            name: token,
            variants: this._data[token],
        };
    }

    parseArrayToken(token) {
        if (this._data[token].length == 1) {
            return this.arrayAST(token);
        } else {
            return this.unionAST(token);
        }
    }

    isFunctionToken(token) {
        const keys = Object.keys(this._data[token]);
        const test = (element) => keys.includes(element);
        return keys.length == 0 || ["arguments", "body", "return"].some(test);
    }

    functionArguments(token) {
        if ("arguments" in this._data[token]) {
            return this._data[token]["arguments"];
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

    functionBody(token) {
        let expressions = [];

        if ("body" in this._data[token]) {
            for (const expression of this._data[token]["body"]) {
                expressions.push(this.expressionAST(expression));
            }
        }

        return expressions;
    }

    functionAST(token) {
        return {
            type: "function",
            name: token,
            arguments: this.functionArguments(token),
            body: this.functionBody(token),
            returnValue: this._data[token]["return"],
        };
    }

    parseObjectToken(token) {
        if (this.isFunctionToken(token)) {
            return this.functionAST(token);
        } else {
            return {};
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
