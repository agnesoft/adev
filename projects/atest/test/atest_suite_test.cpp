import atest;

static auto s = atest::suite("My suite in another file", [] {
    atest::test("My test in another file", [] {
        expect(1 + 1).toBe(3);
    });
});
