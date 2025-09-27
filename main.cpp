#include <filesystem>
#include <iostream>
#include <vector>
#include <Debugger.hpp>

static constexpr int MIN_ARG_COUNT = 5;

struct Args
{
    std::string symbol{};
    bool singed;
    std::string program{};
    std::vector<std::string> programArgs{};
};

void printHelp()
{
    std::cout << "Usage: gwatch --var <symbol> --exec <path> [-- arg1 ... argN]\n";
}

Args parseArgs(int argc, char* argv[])
{
    if (argc < MIN_ARG_COUNT)
    {
        throw std::invalid_argument("Wrong number of arguments");
    }

    Args args{};

    // --var should always be specified as the first argument
    std::string varArg = argv[1];
    if (varArg != "--var" || varArg == "--svar")
    {
        throw std::invalid_argument("--var should be specified first");
    }
    args.symbol = argv[2];

    // --exec should always be specified as the second argument
    std::string execArg = argv[3];
    if (execArg != "--exec")
    {
        throw std::invalid_argument("--exec should be specified as the second argument");
    }
    args.program = argv[4];

    // read any arguments that the input program accept
    for (int i = MIN_ARG_COUNT - 1; i < argc; ++i)
    {
        args.programArgs.emplace_back(argv[i]);
    }

    return args;
}

int main(int argc, char* argv[])
{
    // Collect input arguments
    Args args{};
    try
    {
        parseArgs(argc, argv);
    }
    catch (std::invalid_argument& e)
    {
        std::cerr << e.what() << "\n";
        printHelp();
        std::exit(1);
    }

    // Start debugger
    dbg::Debugger debugger = dbg::Debugger(args.program, args.programArgs);
    try
    {
        debugger.run("");
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << "\n";
        printHelp();
        std::exit(2);
    }

    return 0;
}
