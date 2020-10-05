import fs from "fs";
import { print } from "./common.js";

function main() {
    try {
        const adb = JSON.parse(fs.readFileSync("ADb.json"));
    } catch (e) {
        print(`\nERROR: ${e}\n`);
    }
}

main();
