//
//  g++ -g -o factorial factorial.cpp
//

#include <cstdlib>
#include <iostream>

const int global_var = 42;

int factorial(int n)
{
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }
    int num = std::atoi(argv[1]);
    std::cout << "Factorial of " << num << " is " << factorial(num) << std::endl;
    return 0;
}