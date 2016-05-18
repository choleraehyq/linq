Linq
====

Introduction
------------

Linq is a simplified implementation of LINQ for C++. 

Currently it supports following methods:

*   aggregate(reducer)
*   all(predicate)
*   any(predicate)
*   average()
*   concat(range)
*   contains(element)
*   count()
*   count(predicat)
*   empty()
*   distinct()
*   element_at(index)
*   except(range)
*   find(element)
*   first(predicate)
*   group_by(key_selector)
*   group_by(key_selector, element_selector)
*   keys()
*   last(predicate)
*   max()
*   min()
*   order_by(selector)
*   order_by()
*   order_by_descending()
*   reverse()
*   select(selector)
*   skip(count)
*   skip_while(predicate)
*   sum()
*   take(count)
*   take_while(predicate)
*   values()
*   where(predicate)

Usage
-----

like this:
```cpp
std::vector<int> v{1, 2, 3, 4};
auto r = from(v).select([](int i){ return i+2; }).where([](int i){ return i > 2; }).max();
```
`r` will be 6.

Install
-------

linq is a header-only project. You can simply copy the include/linq.hpp to your directory to use it.

Prerequisite:

*  C++ compiler supporting C++14
*  boost library 1.58.0+

You can run 
```bash
cc linq_test.cc -I /path/to/linq -std=c++14 -lgtest -lpthread -o test
./test
```
to run the test, but you need gtest 1.7.0+ installed first.

Contributions
-------------

All kinds of contributions are welcomed.

License
-------

BSD-2