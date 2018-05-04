# lazy-cartesian-product
A small .hpp library that can generate combinations with a focus on memory-efficiency

## Introduction
When generating data, it may be necessary to create a set of distinct possible combinations. This can be useful for statistics, filling up a database, or running some function over the a combination (like generating cost data given combination `{x, y, z}`). A lot of libraries exist to do this, but memory space can quickly run out when the amount of combinations rises to a very large number.

The library also focuses on using STL utilities from the C++ language without the need for extra overhead (like Boost).

If you would like to see how this library was implemented, [combigen](https://github.com/iamtheburd/combigen) is a CLI tool that helps generate distinct combinations and output them to `.csv` or `.json` format.

## Usage
Input will always be a `vector<vector<string>>`. This keeps things nice and simple, and you can always convert back as necessary in your own projects. There are two public functions available from the library:

* `entry_at` - Generates the *nth* combination at entry `index`
* `generate_samples` - Generates a random (distinct, evenly spread out) subset of possible combinations of size `sample_size`

This project is also licensed under the MIT license, so feel free to use and change this however you please.

## Installation
Simply place the `.hpp` file somewhere in your project and include the class:

```
...
#include "lazy-cartesian-product.hpp"
...


using lazycp::lazy-cartesian-product; // For easier access
...
```

When compiling your project, ensure that it compiles to the C++11 standard (`-std=c++11` flag for GCC). 

## TODOs
* Add better exception-handling
* Add testing framework

## Contributing
Pull-requests are always welcome!

## License
MIT License

Copyright (c) 2018 Tyler Burdsall

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
