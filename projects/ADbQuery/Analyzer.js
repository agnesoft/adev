export default class Analyzer {
    constructor(ast) {
        this._ast = ast;
    }

    verifyName(ast) {
        if (!isNaN(ast["name"][0])) {
            throw `Analyzer: invalid ${ast["type"]} name '${ast["name"]}' (a type name cannot start with a number)`;
        }
    }

    verifyType(type, ast) {
        if (!this.typeExists(type)) {
            throw `Analyzer: unknown type '${type}' referenced in ${ast["type"]} '${ast["name"]}'`;
        }
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

    analyzeObject(ast) {}

    analyze() {
        for (const ast of this._ast) {
            this.verifyName(ast);

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
                default:
                    throw `Analyzer: Unknown type '${ast["type"]}' (name: '${ast["name"]})'`;
            }
        }
    }
}
