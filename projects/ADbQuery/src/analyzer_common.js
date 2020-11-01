export function isNative(type) {
    return ["byte", "int64", "string"].includes(type);
}

export function typeExists(type, ast) {
    return isNative(type) || type in ast;
}

export function realType(type, ast) {
    if (!isNaN(type) || isNative(type)) {
        return type;
    }

    if (!type || !(type in ast)) {
        return undefined;
    }

    if (ast[type]["type"] == "alias") {
        return realType(ast[type]["aliasedType"], ast);
    }

    return type;
}

export function typeType(type, ast) {
    if (!isNaN(type) || isNative(type)) {
        return "native";
    }

    return ast[type]["type"];
}
