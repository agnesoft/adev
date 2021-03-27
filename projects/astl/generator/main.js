import fs from "fs";

const STLHeaders = {
    Concepts: {
        concepts: "20",
    },
    Coroutines: {
        coroutines: "20",
    },
    Utilities: {
        cstdlib: "",
        csignal: "",
        csetjmp: "",
        cstdarg: "",
        typeinfo: "",
        typeindex: "11",
        type_traits: "11",
        bitset: "",
        functional: "",
        utility: "",
        ctime: "",
        chrono: "11",
        cstddef: "",
        initializer_list: "11",
        tuple: "11",
        any: "17",
        optional: "17",
        variant: "17",
        compare: "20",
        version: "20",
        source_location: "20",
    },
    Memory: {
        new: "",
        memory: "",
        scoped_allocator: "11",
        memory_resource: "17",
    },
    Limits: {
        climits: "",
        cfloat: "",
        cstdint: "11",
        cinttypes: "11",
        limits: "",
    },
    Error: {
        exception: "",
        stdexcept: "",
        cassert: "",
        system_error: "11",
        cerrno: "",
    },
    Strings: {
        cctype: "",
        cwctype: "",
        cstring: "",
        cwchar: "",
        cuchar: "11",
        string: "",
        string_view: "17",
        charconv: "17",
        format: "20",
    },
    Containers: {
        array: "11",
        vector: "",
        deque: "",
        list: "",
        forward_list: "11",
        set: "",
        map: "",
        unordered_set: "11",
        unordered_map: "11",
        stack: "",
        queue: "",
        span: "20",
    },
    Iterators: {
        iterator: "",
    },
    Ranges: {
        ranges: "20",
    },
    Algorithms: {
        algorithm: "",
        execution: "17",
    },
    Numerics: {
        cmath: "",
        complex: "",
        valarray: "",
        random: "11",
        numeric: "",
        ratio: "11",
        cfenv: "11",
        bit: "20",
        numbers: "20",
    },
    Localization: {
        locale: "",
        clocale: "",
    },
    IO: {
        iosfwd: "",
        ios: "",
        istream: "",
        ostream: "",
        iostream: "",
        fstream: "",
        sstream: "",
        syncstream: "20",
        iomanip: "",
        streambuf: "",
        cstdio: "",
    },
    Filesystem: {
        filesystem: "17",
    },
    Regexp: {
        regex: "11",
    },
    Atomic: {
        atomic: "11",
    },
    Thread: {
        thread: "11",
        stop_token: "20",
        mutex: "11",
        shared_mutex: "14",
        future: "11",
        condition_variable: "11",
        semaphore: "20",
        latch: "20",
        barrier: "20",
    },
};

function includeGuard() {
    return `//GENERATED FILE. DO NOT MODIFY.

#ifndef ADEV_ASTL_ASTL_HPP
#define ADEV_ASTL_ASTL_HPP`;
}

function categoryHeading(category) {
    return `\n//${category}`;
}

function stlHeader(name, cppVersion) {
    return `
#if __has_include(<${name}>)
#    include <${name}>${cppVersion ? ` //C++${cppVersion}` : ""}
#else
#    ifdef _MSC_VER
#        pragma message("<${name}> not available${cppVersion ? ` (C++${cppVersion})` : ""}")
#    else
#        warning "<${name}> not available${cppVersion ? ` (C++${cppVersion})` : ""}"
#    endif
#endif
`;
}

function stlCategory(name, category) {
    let buffer = categoryHeading(name);

    for (const header in category) {
        buffer += stlHeader(header, category[header]);
    }

    return buffer;
}

function stlIncludes() {
    let buffer = "";

    for (const category in STLHeaders) {
        buffer += stlCategory(category, STLHeaders[category]);
    }

    return buffer;
}

function main() {
    let header = `${includeGuard()}
${stlIncludes()}
#endif
`;

    let moduleMap = `//GENERATED FILE. DO NOT MODIFY.

module astl_hpp {
    header "astl.hpp"
    export *
}
`;

    fs.writeFileSync("../include/astl.hpp", header);
    fs.writeFileSync("../include/module.modulemap", moduleMap);
}

main();
