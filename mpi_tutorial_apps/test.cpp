#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "There is " << argc << " argument given." << std::endl;

    for (int i = 0; i < argc; i++)
        std::cout << argv[i] << " is the argument number " << i << std::endl;
    return 0;
}
//mpic++ test.cpp -o test
//./test 55 16