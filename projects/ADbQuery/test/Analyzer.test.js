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
    test("[unknown type]", () => {
        const ast = [
            {
                name: "MyType",
                type: "AwesomeType",
            },
        ];
        const analyze = () => {
            new Analyzer(ast).analyze();
        };
        expect(analyze).toThrow(
            "Analyzer: Unknown type 'AwesomeType' (name: 'MyType')."
        );
    });

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
                    "Analyzer: unknown type 'Obj' referenced in alias 'MyObj'."
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
                "Analyzer: unknown type 'Obj' referenced in array 'MyArr'."
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
                "Analyzer: unknown type 'MyArr' referenced in variant 'MyVariant'."
            );
        });
    });

    describe("[object]", () => {
        describe("[base]", () => {
            test("[unknown base]", () => {
                const data = {
                    Obj: { base: "SomeType" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the base 'SomeType' of object 'Obj' is not an existing type."
                );
            });

            test("[non-object base]", () => {
                const data = {
                    SomeArray: ["int64"],
                    Obj: { base: "SomeArray" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the base 'SomeArray' of object 'Obj' is of type 'array' (must be 'object')."
                );
            });

            test("[valid object base]", () => {
                const data = {
                    SomeType: {},
                    Obj: { base: "SomeType" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[aliased object base]", () => {
                const data = {
                    BaseObj: {},
                    BaseAlias: "BaseObj",
                    Obj: { base: "BaseAlias" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[aliased cascade object base]", () => {
                const data = {
                    BaseObj: {},
                    InterAlias: "BaseObj",
                    BaseAlias: "InterAlias",
                    Obj: { base: "BaseAlias" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[aliased object base with wrong type]", () => {
                const data = {
                    Func: { body: [] },
                    BaseAlias: "Func",
                    Obj: { base: "BaseAlias" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the base 'BaseAlias' of object 'Obj' is of type 'function' (must be 'object')."
                );
            });

            test("[aliased object base with wrong type]", () => {
                const data = {
                    Func: { body: [] },
                    InterAlias: "Func",
                    BaseAlias: "InterAlias",
                    Obj: { base: "BaseAlias" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the base 'BaseAlias' of object 'Obj' is of type 'function' (must be 'object')."
                );
            });

            test("[aliased object base with missing type]", () => {
                const data = {
                    BaseAlias: "InterAlias",
                    Obj: { base: "BaseAlias" },
                    InterAlias: "Func",
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the base 'BaseAlias' of object 'Obj' is of type 'undefined' (must be 'object')."
                );
            });
        });

        describe("[fields]", () => {
            test("[existing types]", () => {
                const data = {
                    MyArray: ["int64"],
                    Id: "int64",
                    Obj: {
                        fields: ["MyArray", "Id"],
                    },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[missing types]", () => {
                const data = {
                    MyArray: ["int64"],
                    Obj: {
                        fields: ["MyArray", "Id"],
                    },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the field 'Id' of object 'Obj' is not an existing type."
                );
            });
        });
    });

    describe("[functions]", () => {
        describe("[arguments]", () => {
            test("[existing types]", () => {
                const data = {
                    Id: "int64",
                    MyArray: ["int64"],
                    foo: { arguments: ["Id", "MyArray"] },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[missing types]", () => {
                const data = {
                    Id: "int64",
                    foo: { arguments: ["Id", "MyArray"] },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the argument 'MyArray' of function 'foo' is not an existing type."
                );
            });
        });

        describe("[return]", () => {
            test("[existing type]", () => {
                const data = {
                    Id: "int64",
                    foo: { return: "Id" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).not.toThrow();
            });

            test("[missing type]", () => {
                const data = {
                    foo: { return: "Id" },
                };
                const analyze = () => {
                    new Analyzer(new Parser(data).parse()).analyze();
                };
                expect(analyze).toThrow(
                    "Analyzer: the return value 'Id' of function 'foo' is not an existing type."
                );
            });
        });
    });
});
