//
//  g++ -g -o cli_example cli_example.cpp
//

#include <chrono>
#include <thread>
#include <random>

unsigned char a = 0;
short b = 0;
int c = 0;

// increments a once per second
void increment()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ++a;
    }
}

// decrements b once per second
void decrement()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        --b;
    }
}

// sets c to random value once a second
void randomT()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);

    while (true)
    {
        int32_t random_num = dist(gen);
        c = random_num;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    std::thread incThread(increment);
    std::thread decThread(decrement);
    std::thread randThread(randomT);

    incThread.join();
    decThread.join();
    randThread.join();

    return 0;
}

