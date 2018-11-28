# lazy-cartesian-product
A small .hpp library that can generate combinations with a focus on memory-efficiency

## Introduction
When generating data, it may be necessary to create a set of distinct possible combinations. This can be useful for statistics, filling up a database, or running some function over the a combination (like generating cost data given combination `{x, y, z}`). A lot of libraries exist to do this, but memory space can quickly run out when the amount of combinations rises to a very large number.

This library also provides the flexibility of operating on VERY large sets of data. If you have [Boost](https://www.boost.org) installed, you can utilize its features and operate on numbers from 0 - 2^1024 - 1. (See the **Installation** section for more).

If you would like to see how this library was implemented, [combigen](https://github.com/iamtheburd/combigen) is a CLI tool that helps generate distinct combinations and output them to `.csv` or `.json` format.

## Usage
Input will always be a `vector<vector<string>>`. This keeps things nice and simple, and you can always convert back as necessary in your own projects. There are some public functions available from the library:

* `entry_at` - Generates the *nth* combination at entry `index`
* `generate_samples` - Generates a random (distinct, evenly spread out) subset of possible combinations of size `sample_size`
* `compute_max_size` - Computes the maximum amount of possible combinations
* `generate_random_indices` - Given the desired sample size and the maximum size, this function will return a `vector` containing an evenly-distributed list of indices throughout the range given.

If you use the `boost` library, all of the above functions will instead be prepended with `boost_` (see more in **Example Usage**).

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

When compiling your project, ensure that it compiles to the C++14 standard (`-std=c++14` flag for g++). 

## Prerequisites:
You will need the following installed before including this library into your project:

### Linux:
* C++ compiler capable of compiling to the C++14 or higher standard
* [boost](https://www.boost.org) if you plan to operate with large sets of data/numbers

You can also use your distro's package manager to install the necessary Boost libraries:

#### Debian/Ubuntu
`$ sudo apt install libboost-all-dev`

#### Fedora
`$ sudo dnf install boost`

#### Arch/Manjaro/Antergos
`$ sudo pacman -Sy boost`


### Windows:
* Visual Studio 2015+
* Boost, either installed system-wide or you've downloaded the precompiled libraries for your systema and placed them somewhere easy to remember

### Compiling with Boost functionality
If you need to operate on larger sets of data, it is recommended to install the `boost` library. Follow the appropriate instructions for your operating system to install `boost` onto your machine.

#### Linux/UNIX/Cygwin
Assuming you had one file called `main.cpp`, you can simply compile with the following line:

```
$ g++ -Wall -O2 -std=c++14 -DUSE_BOOST main.cpp -lboost_random  # Note: it is crucial the -lboost_random flag comes last
```

#### Windows
1. Open up the x64 version of the Visual Studio compiler (usually found under Start Menu -> Visual Studio 20XX -> x64 Native Tools Command Propmpt for VS 20XX), with XX being your installed version
2. Navigate to the directory where your project is located
3. Assuming you are just straight-compiling your source files and downloaded the precompiled libraries, enter this command:
```
cl /EHsc /DUSE_BOOST /I C:\path\to\boost_1_68_0 your_file.cpp /link /LIBPATH:C:\path\to\boost_1_68_0\lib64-msvc-14.1
```

If you would like to see a more complex compilation example take a look at the [combigen Makefile](https://github.com/iamtheburd/combigen/blob/master/Makefile).

## Example Usage
While `lazy-cartesian-product` can be used in many situations, below are a few examples showcasing some of the functions of the library:

### Basic Example (without Boost)
First, we can generate a possible combination at the *nth* entry. For this example, we can find the combination of pizza components at the 5th entry.

```cpp
#include <string>
#include <vector>
#include <iostream>
#include "lazy-cartesian-product.hpp"   // Assumes the .hpp file is in the same directory

using std::cout;
using std::vector;

using lazycp::lazy_cartesian_product;   // For easier readability


int main (int argc, char* argv[])
{
     vector<string> crusts   = { "Thin", "Thick", "Cheese-stuffed", "Garlic-rubbed" };
     vector<string> sauces   = { "Marinara", "BBQ", "None" };
     vector<string> cheeses  = { "Mozarella", "Cheddar", "Parmesan" };
     vector<string> toppings = { "Extra Cheese", "Chicken", "Pepperoni", "Sausage" };

     // Put all of the vectors together so lazy-cartesian-product can accept the input
     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };

     // Show the combination at the 5th index
     vector<string> result = lazy_cartesian_product::entry_at(possibilities, 5);

     // Display this result:
     //    "Mozarella, Thin, BBQ, Chicken, "
     for (const string &entry: result)
     {
        cout << result << ", ";
     }
     cout << "\n";
 
     return 0;
}
```


### Generate Random Sample (Performance, without Boost)
Next, we will generate a random sample of combinations. This is especially useful
for data analysis when data needs to be generated in an evenly-distributed manner.
This example focuses on high-performance at the expense of memory usage (for large-computations, of course).

```cpp
#include <string>
#include <vector>
#include <iostream>
#include "lazy-cartesian-product.hpp"

using std::cout;
using std::vector;

using lazycp::lazy_cartesian_product;   


int main (int argc, char* argv[])
{
     vector<string> crusts   = { "Thin", "Thick", "Cheese-stuffed", "Garlic-rubbed" };
     vector<string> sauces   = { "Marinara", "BBQ", "None" };
     vector<string> cheeses  = { "Mozarella", "Cheddar", "Parmesan" };
     vector<string> toppings = { "Extra Cheese", "Chicken", "Pepperoni", "Sausage" };

     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };
    
     // Generate 10 random samples
     vector<vector<string>> random_samples = lazy_cartesian_product::generate_samples(possibilities, 10);

     /* Displayed these results:
            Mozarella, Thin, Marinara, Sausage,
            Mozarella, Cheese-stuffed, Marinara, Chicken,
            Mozarella, Cheese-stuffed, None, Extra Cheese,
            Cheddar, Thin, None, Chicken
            Cheddar, Thick, Marinara, Extra Cheese
            Cheddar, Cheese-stuffed, None, Sausage
            Cheddar, Garlic-rubbed, BBQ, Sausage
            Cheddar, Garlic-rubbed, None, Extra Cheese
            Parmesan, Thin, BBQ, Extra Cheese
            Parmesan, Cheese-stuffed, BBQ, Chicken
      */
     for (const vector<string> &entry: random_samples) 
     {
        for (const string &item: entry)
        {
            cout << item << ", ";
        }
        cout << "\n";
     }
     return 0;
}
```

### Generate Random Sample (Memory-optimized, without Boost)
Finally, we can generate a random sample of combinations but do so without
consuming a lot of memory. By using some of the provided functions, we can
sacrifice a small amount of computing time so that very little memory space
is used. As such, the only limiters that remain are I/O performance and availability
of disk space. This is especially useful for lower-end machines.


```cpp
#include <string>
#include <vector>
#include <iostream>
#include "lazy-cartesian-product.hpp"

using std::cout;
using std::vector;

using lazycp::lazy_cartesian_product;   


int main (int argc, char* argv[])
{
     vector<string> crusts   = { "Thin", "Thick", "Cheese-stuffed", "Garlic-rubbed" };
     vector<string> sauces   = { "Marinara", "BBQ", "None" };
     vector<string> cheeses  = { "Mozarella", "Cheddar", "Parmesan" };
     vector<string> toppings = { "Extra Cheese", "Chicken", "Pepperoni", "Sausage" };

     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };

     // First, find the max size of the combinations
     unsigned long long max_size = lazy_cartesian_product::compute_max_size(possiblities);

     // Next, generate a list of random indices to find
     vector<unsigned long long> random_indices = lazy_cartesian_product::generate_random_indices(100, max_size);
    
     // Finally, iterate through the random indices, generate a combination, and display it
     for (const unsigned long long &index: random_indices)
     {
        vector<string> entry = lazy_cartesian_product::entry_at(possibilities, index);
        for (const string &item: entry)
        {
            cout << item << ", ";
        }
        cout << "\n";
     }
     return 0;
}
```


## Example Usage (with Boost)
The following examples demonstrate the same functionality as above, but with the Boost library installed.


### Basic Example (with Boost)
First, we can generate a possible combination at the *nth* entry. For this example, we can find the combination of pizza components at the 5th entry.

```cpp
#include <string>
#include <iostream>
#include "lazy-cartesian-product.hpp"   // Assumes the .hpp file is in the same directory
#include <boost/container/vector.hpp>  // Notice how we now use the Boost libraries

using std::string;
using std::cout;
using boost::container::vector;

using lazycp::lazy_cartesian_product;   // For easier readability

int main (int argc, char* argv[])
{
     vector<string> crusts   = { // Some REALLY big amount };
     vector<string> sauces   = { ... };
     vector<string> cheeses  = { ... };
     vector<string> toppings = { ... };

     // Put all of the vectors together so lazy-cartesian-product can accept the input
     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };

     // Show the combination at the 18909849082309840923850987th index
     string index("18909849082309840923850987"); // Now we instead pass in a string
     vector<string> result = lazy_cartesian_product::boost_entry_at(possibilities, index);

     // Display result:
     for (const string &entry: result)
     {
        cout << result << ", ";
     }
     cout << "\n";
 
     return 0;
}
```


### Generate Random Sample (Performance, with Boost)

```cpp
#include <string>
#include <iostream>
#include "lazy-cartesian-product.hpp"
#include <boost/container/vector.hpp>  

using std::string;
using std::cout;
using boost::container::vector;

using lazycp::lazy_cartesian_product;   


int main (int argc, char* argv[])
{
     vector<string> crusts   = { // Some REALLY big amount };
     vector<string> sauces   = { ... };
     vector<string> cheeses  = { ... };
     vector<string> toppings = { ... };

     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };
    
     // Generate an incredibly large sample size
     string sample_size = "478928739482739877729118939847";
     vector<vector<string>> random_samples = lazy_cartesian_product::boost_generate_samples(possibilities, sample_size);

     for (const vector<string> &entry: random_samples) 
     {
        for (const string &item: entry)
        {
            cout << item << ", ";
        }
        cout << "\n";
     }
     return 0;
}
```

### Generate Random Sample (Memory-optimized, with Boost)

```cpp
#include <string>
#include <iostream>
#include "lazy-cartesian-product.hpp"
#include <boost/container/vector.hpp>  
#include <boost/multiprecision/cpp_int.hpp>    // Now we need the cpp_int library

using std::string;
using std::cout;
using boost::container::vector;
using boost::multiprecision::uint1024_t;       // This way we can compute the max size

using lazycp::lazy_cartesian_product;   

int main (int argc, char* argv[])
{
     vector<string> crusts   = { // Some REALLY big amount };
     vector<string> sauces   = { ... };
     vector<string> cheeses  = { ... };
     vector<string> toppings = { ... };


     vector<vector<string>> possibilities = { crusts, sauces, cheeses, toppings };

     // First, find the max size of the combinations
     uint1024_t max_size = lazy_cartesian_product::boost_compute_max_size(possiblities);

     // Next, generate a list of random indices to find (but of type uint1024_t)
     string sample_size = "478928739482739877729118939847";
     vector<uint1024_t> random_indices = lazy_cartesian_product::boost_generate_random_indices(sample_size, max_size);
    
     // Finally, iterate through the random indices, generate a combination, and display it
     for (const uint1024_t &index: random_indices)
     {
        // Note: we need to convert the index to a string to find the entry_at, so we use the convert_to function
        vector<string> entry = lazy_cartesian_product::boost_entry_at(possibilities, index.convert_to<string>()); 
        for (const string &item: entry)
        {
            cout << item << ", ";
        }
        cout << "\n";
     }
     return 0;
}
```

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
