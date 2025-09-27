//
//  g++ -g -o arg_printer arg_printer.cpp
//

#include <iostream>

const int global_var = 42;

int main(int argc, char* argv[])
{
    std::cout << "Program name: " << argv[0] << std::endl;
    std::cout << "Arguments:" << std::endl;
    for (int i = 1; i < argc; ++i)
    {
        std::cout << "Arg " << i << ": " << argv[i] << std::endl;
    }
    return 0;
}