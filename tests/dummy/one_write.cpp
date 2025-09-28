//
//  g++ -g -o one_write one_write.cpp
//

long global_var = 42;

int main()
{
    global_var = 142;
    return 0;
}