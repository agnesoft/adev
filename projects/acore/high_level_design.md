# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ACore](#acore)

## Problem

There is a common need for generic functionality (command line handling, logging etc.) that is required by most projects and that is not readily available in Standard Template Library (STL) that ships with every major compiler toolchain.

## Requirements

-   Used as C++20 module.
-   Cross platform (Linux, Windows, Unix).
-   Cross platform API (no platform specifics in the API).

## Existing Solutions

There are plenty of cross platform libraries out there. `Qt 6` satisfies the corss platformness and cleanness of API the most. However it is strictly C++17 and some of its design choices (inheritance and virtual calls) are grossly outdated nowadays impacting performance. `Poco` is another option but it is in many areas simply incomplete and requires significant effort to use its components by requiring users to implement parts of the API themselves. It is also not C++20 yet (it is in fact only C++14). `Boost` is probably most widely used library beside STL and often serves as a staging ground for official STL features. The STL API itself is less than ideal and Boost's is often even worse being extremely hard to understand and to use. Its quality and maintanance varies from one library to the next with many parts being basically abandoned. The issue with the large library collections is also that they contain lots of stuff that is not really needed. It becomes an overkill to start using Qt or Boost to make use of one or two features.

Aside from huge libraries there are standalone solutions for each problem. They tend to differ a lot in all aspects and would require wrappers to bring them to the common standard. Nevertheless some standalone libraries are such high quality and with superb maintenance that implementing them anew would be folly (e.g `rapidjson`).

## ACore

Agnesoft Core is the base library written in C++20 for all other projects that provide generic functionality with consistent API. It does utilize third party libraries and abstract the users from their specific APIs by providing a wrapper. It is consumed as a module.
