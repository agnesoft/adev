export function jsType(token) {
    if (Array.isArray(token)) {
        return "array";
    } else {
        return typeof token;
    }
}
