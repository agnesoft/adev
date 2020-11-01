# ADbQuery

Node.js code generator of language bindings for ADb. The ADb.json is the main schema of the bindings. It describes known types, their data, layout and functions (methods). It also describes the "builders" as chained objects to build the queries (see [builder pattern](https://en.wikipedia.org/wiki/Builder_pattern)). Each language binding is generated from this schema to ensure that any change to it propagates seamlessly to all supported language bindings.

## Schema

-   The root of the schema is an object.
-   Each key is the unique name of a type.
-   Each type can be one of the following:
    -   "alias"
    -   "array"
    -   "function"
    -   "object"
    -   "variant"

#### alias

-   Represents an alias of a different type (including another alias) defined in the schema or of a native type.
-   Supported native types:
    -   `byte`
    -   `int64`
    -   `string`

Syntax:

```
"AliasName": "AliasedType"
```

AST:

```
{
    type: "alias",
    name: "AliasName",
    aliasedType: "AliasedType"
}
```

#### array

-   Represents an array of another type (custom, native).

Syntax

```
"MyArr": ["ArrayType"]
```

AST:

```
{
    type: "array",
    name: "MyArr",
    arrayType: "ArrayType"
}
```

#### function

-   Represents a function.
-   The key is the function name (applies both to stand alone functions and _object_ functions a.k.a. methods).
-   Must have a key "body" that must be an array with only string elements. Each element represents an _expression_ (see below). It can be empty.
-   May have a key "arguments" that must be an array with only string elements that must be one of the other defined types.
-   May have a key "return" that must be a string and its value must be one of the other defined types.

Syntax

```
"foo": {
    "arguments": ["ArgType1"], //optional
    "body": [ "return ArgType1" ], //see below
    "return": "ArgType1" //optional
}
```

AST:

```
{
    type: "function",
    name: "foo",
    arguments: ["ArgType1],
    body: [ {
        type: "return",
        value: "ArgType1",
        returnType: "argument"
    } ],
    return: "string"
}
```

#### object

-   Represents a compound type.
-   May have a key "fields" that must be an array with only string elements that must be one of the other defined types in the schema.
-   May have a key "base" that represents this type's parent. The value must be a string and must be one of the other objects previously defined.
-   May have a key "functions" that represents the object's methods. The value must be an object with the keys being the functions' names. See _function_ for defails on each entry in this object.

Syntax:

```
"Obj": {
    "base": "BaseObj",
    "fields": ["FieldType1", "FieldType2"],
    "functions: {
        "foo": { body: [] }
    }
}
```

AST

```
{
    type: "object",
    name: "Obj",
    base: "BaseObj",
    fields: [ "FiedlType1", "FieldType2" ],
    functions: [
        {
            type: "function",
            name: "foo",
            arguments: [],
            body: []
            returnValue: undefined
        }
    ]
}
```

#### variant

-   Represents a union type (a type that can be one of the other predefined types).

Syntax

```
"MyVariant": ["Var1", "Var2"]
```

AST

```
{
    type: "variant",
    name: "MyVariant",
    variants: ["Var1", "Var2"]

}
```

## Schema Expressions

Each _function_ can have a key "body" that consists of one or more string expressions. Each expression is written in a kind of simplistic special purpose pseudo code that is then converted into AST. This pseudo code relies heavily on context and inference to be as brief as possible. It is designed solely to represent the builder pattern for ADb queries and does not accomodate any other purpose. The rules and syntax are as follow:

```
type(.type...) = type(.type...)
return type
```

-   Each expression can only consist of defined types.
-   The "." is a type delimiter.
-   Each consecutive type after "." must make sense in regards to its parent type. It must be either:
    -   The "type" of an array if the parent is of type "array".
    -   One of the "fields" of an object if the parent is of type "object".
    -   One of the "variants" of variant if the parent is of type "variant".
-   Both sides of an assignment must be compatible. One of the following must be true:
    -   Last element on both sides must be of the same type.
    -   Last element on the right hand side must be a "type" of an array if the last element on the left hand side is of kind "array". **In this case the right hand side is appended to the array**.
    -   Last element on the right hand side must be one of the "variants" of an union if the last element on the right hand side is of kind "union".
    -   Right hand side is a number.
-   The "alias" types are expanded to their original type for the purposes of determining their _parent_ properties.
-   For functions in objects:
    -   If the first type in a type sequence is the same as any of this object's or its parent object's "fields" it will be treated as such (being a reference to the field).
-   For all functions:
    -   If the first type in a type sequence is the same as one of the "arguments" or previously declared expressions it will be treated as such (referencing the argument).
    -   If the object is not any of the "base", "fields" or "arguments" or previously declared expressions it will be considered a **new declaration**.

Example:

```
"Query.QueryData.InsertNodesQueryData.ElementsDataField = ElementsDataField"
```

In the example:

-   The first element is of type "Query (object)". It likely references a "base" or one of the "fields" of the object. If not it will be a new declaration.
-   The second segment references Query's field "QueryData (union)".
-   The third segment references the QueryData's variant "InsertNodesQueryData (object)". It can be assumed the union holds currently this variant. If it is a new declaration the union will be initialized to this variant.
-   Finally the fourth segment references the variant InsertNodesQueryData's field "ElementsDataField" and assigns to it a value of the same type that very likely represents an argument of the function.

## Serialization

Every native and custom type is serialized in the following layout. The serialization is often language specific and native types used dependent and therefore there is no AST generated. It may take form of stream operators, stand-alone functions or be directly mapped to memory (no actual serialization needed then).

#### native

_byte_

As is.

_int64_

Converted to(from) little-endian and stored as exactly 4 bytes.

_string_

Size of the string in bytes as `int64` followed by the characters as `byte`s. Note that in case the native string have 2-byte characters it needs to be taken into account (i.e. the serialized size will be `size * 2` bytes).

#### array

Size of the array in bytes as `int64` followed by the serialized values.

### function

Not supported.

### object

Individual fields serialized after each other in the order of declaration without padding. Base class' fields are serialized first (recursively).

### variant

Index of currently active variant as `byte` (int8) followed by the serialized active variant.
