export default class Analyzer {
    constructor(ast) {
        this._ast = ast;
    }

    verifyType(type, ast) {
        if (!this.typeExists(type)) {
            throw `Analyzer: unknown type '${type}' referenced in ${ast["type"]} '${ast["name"]}'.`;
        }
    }

    findAST(name) {
        for (const ast of this._ast) {
            if (ast["name"] == name) {
                return ast;
            }
        }

        return undefined;
    }

    typeExists(type) {
        return (
            ["int64", "byte"].includes(type) ||
            this._ast.find((value) => value["name"] == type) != undefined
        );
    }

    analyzeAlias(ast) {
        this.verifyType(ast["aliasedType"], ast);
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

    analyzeFunction(ast) {}

    analyze() {
        for (const ast of this._ast) {
            switch (ast["type"]) {
                case "alias":
                    this.analyzeAlias(ast);
                    break;
                case "array":
                    this.analyzeArray(ast);
                    break;
                case "variant":
                    this.analyzeVariant(ast);
                    break;
                case "object":
                    this.analyzeObject(ast);
                    break;
                case "function":
                    this.analyzeFunction(ast);
                    break;
                default:
                    throw `Analyzer: Unknown type '${ast["type"]}' (name: '${ast["name"]}').`;
            }
        }

        return this._ast;
    }
}
