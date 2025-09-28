//
//  g++ -g -o read_thread read_thread.cpp
//

#include <thread>
#include <iostream>

int global_var = 42;

int main()
{
    std::thread t(
        []()
        {
            std::thread tIndirect(
                []()
                {
                    for (int i = 0; i < 100000; ++i)
                    {
                        std::cout << global_var << "\n";
                    }
                });

            tIndirect.join();
        });

    t.join();

    return 0;
}