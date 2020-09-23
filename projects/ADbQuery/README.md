# ADbQuery

Node.js code generator of language bindings for ADb. The Adb.json is the main schema of the bindings. It describes known types, their data and layout and functions (methods). It also describes the "builders" to create the Query them. Each language binding is generated from this schema to ensure that any change to it seamlessly propagate to all supported languages.

## Schema

-   The root of the schema is object.
-   Each key is the unique name of a type.
-   Each type must have the "kind" key that can have only a string value.
    -   "alias"
    -   "array"
    -   "function"
    -   "native"
    -   "object"
    -   "union"

#### alias

-   Represents an alias of a different type defined in the schema.
-   Must have a key "type" that can have only a string value and the value must be any of the other defined types in the schema.

#### array

-   Represents an array of other types.
-   Must have a key "type" that can have only string value and must be any of the other defined types in the schema.
-   May have a key "length" that signifies a fixed length array and its value must be a number.

#### function

-   Represents a function.
-   The key is the function name (applies both to stand alone functions and _object_ functions).
-   May have a key "arguments" that must be an array with only string elements that must be any of the other defined types in the schema.
-   May have a key "body" that must be an array with only string elements. Each element represents an _expression_ (see below).
-   May have a key "return" that must be a string and its value must be any of the other defined types in the schema.

#### native

-   Represents an alias to an in-built type.
-   Must have a key "type" that can have only a string value.
    -   int64
    -   string

#### object

-   Represents a compound type.
-   May have a key "fields" that must be an array with only string elements that must be any of the other defined types in the schema.
-   May have a key "base" that represents this type's parent. The value must be a string and must be another object defined in the schema.
-   May have a key "functions" that represents this type's methods. The value must be an object with keys being the functions' names. See _function_ for defails on each entry in this object.

#### union

-   Represents a union type (a type that can be one of other types).
-   Must have a key "variants" that must be an array with only string elements. Each element must be another type defined in the schema.

## Schema Expressions

Each _function_ can have a key "body" that consists of one or more string expressions. Each expression is written in a kind of simplistic special purpose pseudo code that is then converted into all supported programming languages. This pseudo code relies heavily on context and inference to be as brief as possible. It is designed solely to represent the builder pattern for ADb queries and does not accomodate any other purpose. The rules and syntax are as follow:

```
type(.type...) = type(.type...)
```

-   Each expression can only consist of defined types.
-   Each expression is an assignment and must contain an "=" sign.
-   The "." is a type delimiter.
-   Each consecutive type after "." must make sense in regards to its parent type. It must be either:
    -   The "type" of an array if the parent is of type "array".
    -   One of the "fields" of an object if the parent is of type "object".
    -   One of the "variants" of an union if the parent is of type "union".
-   Both sides of an assignment must be compatible. One of the following must be true:
    -   Last element on both sides must be of the same type.
    -   Last element on the right hand side must be a "type" of an array if the last element on the left hand side is of type "array". **In this case the right hand side is appended to the array**.
    -   Last element on the right hand side must be one of the "variants" of an union if the last element on the right hand side is of type "union".
    -   Right hand side is a number.
-   The "alias" types are expanded to their original type for the purposes of determining their _parent_ properties.
-   For functions in objects:
    -   If the first type in a type sequence is the same as any of the parent object's "base" key or one of the "fields" it will be treated as such (being a reference to either the base or the field).
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
