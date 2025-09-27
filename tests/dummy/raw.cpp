//
//  g++ -g -o one_read one_read.cpp
//

#include <cstdlib>
#include <iostream>

long global_var = 42;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }
    int num = std::atoi(argv[1]);

    for (size_t i = 0; i < num; ++i)
    {
        // same amount of reads and writes
        if (i % 2 == 0)
        {
            int a = global_var; // read
        }
        else
        {
            global_var = 42;   // write
        }
    }
}