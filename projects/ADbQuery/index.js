import fs from "fs";
import { print } from "./common.js";
import Parser from "./Parser.js";

function main() {
    try {
        const adb = JSON.parse(fs.readFileSync("ADb.json"));
        const parse = new Parser();
    } catch (e) {
        print(`\nERROR: ${e}\n`);
    }
}

main();
