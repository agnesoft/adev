import Analyzer from "../Analyzer.js";
import Parser from "../Parser.js";

describe("Analyzer(ast)", () => {
    test("[ast]", () => {
        const constructAnalyzer = () => {
            new Analyzer([]);
        };
        expect(constructAnalyzer).not.toThrow();
    });
});

describe("analyze()", () => {
    describe("[alias]", () => {
        test("[native]", () => {
            const data = {
                Size: "int64",
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).not.toThrow();
        });

        test("[type]", () => {
            const data = {
                Obj: {},
                MyObj: "Obj",
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).not.toThrow();
        });

        test("[missing type]", () => {}),
            test("[unknown type]", () => {
                const data = {
                    MyObj: "Obj",
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: unknown type 'Obj' referenced in alias 'MyObj'"
                );
            });
    });

    describe("[array]", () => {
        test("[native]", () => {
            const data = {
                String: ["byte"],
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).not.toThrow();
        });

        test("[type]", () => {
            const data = {
                Obj: {},
                MyAr: ["Obj"],
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).not.toThrow();
        });

        test("[unknown type]", () => {
            const data = {
                MyArr: ["Obj"],
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).toThrow(
                "Analyzer: unknown type 'Obj' referenced in array 'MyArr'"
            );
        });
    });

    describe("[variant]", () => {
        test("[existing types]", () => {
            const data = {
                MyObj: {},
                MyArr: ["MyObj"],
                MyVariant: ["int64", "MyObj", "MyArr", "byte"],
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).not.toThrow();
        });

        test("[unknown type]", () => {
            const data = {
                MyObj: {},
                MyVariant: ["int64", "MyObj", "MyArr", "byte"],
            };
            const analyze = () => {
                new Analyzer(new Parser(data).parse()).analyze();
            };
            expect(analyze).toThrow(
                "Analyzer: unknown type 'MyArr' referenced in variant 'MyVariant'"
            );
        });
    });
});
