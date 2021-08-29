# Contributing
## Contribute in a branch other than main.
## As a code convention, use .c++ and .h++ extensions for your C++ code and headers.
## Keep whitespace to a minimum (add whitespace where it increases readability, is necessary, or prevents the killing of puppies)
  for example:
  ```
  #include<iostream>
  
  int main(const int argc, const char**const argv){
    std::cout<<"Hello, World!"<<std::endl;
    return 0;
  }
  ```
  is preferred.
  - The blank line between `#include<iostream>` and `int main(...` is necessary for readability. 
  - The whitespace typically present between either `char`and `**` or `**` and `const` is unnecessary and does not increase readability in this case, so it is removed, 
  same for the spaces normally present in `std::cout<<"Hello, World!"`. 
  - Eliminating the tabs before `std::cout` and `return 0` would kill puppies, so it is kept.
  
  A further example:
  ```
  std::cout<<std::left<<std::setw(outputwidth)<<resources["results.header"];
  std::cout<<std::right<<std::setw(std::string(instruction).size())<<instruction<<std::endl;
  
  std::cout<<std::left<<std::setw(outputwidth)<<resources["results.instructions.count"];
  std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[0]<<std::endl;
  ```
  Here, an extra space is inserted after the `std::endl;` because it provides a more readable experience and shows how the pairs of lines exist in relation to one another.
