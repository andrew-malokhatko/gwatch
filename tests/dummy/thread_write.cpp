//
//  g++ -g -o write_thread write_thread.cpp
//

#include <thread>
#include <iostream>

int global_var = 42;

int main()
{
    std::thread t(
        []()
        {
            global_var = 142;
        });

    t.join();

    return 0;
}