function expressionType(part) {
    if (isNaN(part)) {
        return "type";
    } else {
        return "number";
    }
}

function expressionValue(part) {
    if (isNaN(part)) {
        return part;
    } else {
        return Number(part);
    }
}

function partAST(part) {
    return {
        type: expressionType(part),
        value: expressionValue(part),
    };
}

function expressionASTBuilder(part, current) {
    part["parent"] = partAST(current.trim());
    return part["parent"];
}

function sideAST(side) {
    let ast = {};
    const parts = side.split(".");
    parts.reduceRight(expressionASTBuilder, ast);
    return ast["parent"];
}

export function expressionAST(expression) {
    const sides = expression.split("=");

    return {
        type: "assignment",
        left: sideAST(sides[0].trim()),
        right: sideAST(sides[1].trim()),
    };
}
