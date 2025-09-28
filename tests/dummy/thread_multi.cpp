//
//  g++ -g -o read_thread read_thread.cpp
//

#include <thread>

int global_var = 42;

int main()
{
    std::mutex mut {};

    auto func = [&mut]()
    {
        for (int i = 0; i < 10000; ++i)
        {
            mut.lock();
            ++global_var;
            mut.unlock();

            std::this_thread::yield();
        }
    };

    std::thread first(func);
    std::thread second(func);

    first.join();
    second.join();

    return 0;
}