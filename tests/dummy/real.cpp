//
//  g++ -g -o one_read one_read.cpp
//

#include <cstdlib>
#include <iostream>

long global_var = 0;

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
        // simulate some work
        for (int k = 0; k < 50000; ++k)
            ;

        // same amount of reads and writes
        if (i % 2 == 0)
        {
            long a = global_var;   // read
        }
        else
        {
            global_var = 42;   // write
        }
    }
}