//
//  g++ -g -o one_read one_read.cpp
//

#include <iostream>

short global_var = 42;

int main()
{
    std::cout << "global_var: " << global_var << std::endl;
    return 0;
}