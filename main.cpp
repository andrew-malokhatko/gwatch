#include <Debugger.hpp>
#include <iostream>
#include <vector>

static constexpr int MIN_ARG_COUNT = 5;

struct Args
{
    dbg::Variable var{};
    std::string path{};
    std::vector<std::string> args{};
};

void printHelp()
{
    std::cout << "Usage: gwatch (--var | --svar) <symbol> --exec <path> [-- arg1 ... argN]\n";
}

Args parseArgs(int argc, char* argv[])
{
    if (argc < MIN_ARG_COUNT)
    {
        throw std::invalid_argument("Wrong argument count");
    }

    Args args{};

    // --var should always be specified as the first argument
    std::string varArg = argv[1];
    if (varArg != "--var" && varArg != "--svar")
    {
        throw std::invalid_argument("--var should be specified first");
    }

    bool isSigned = varArg == "--svar";
    args.var = dbg::Variable(argv[2], isSigned);

    // --exec should always be specified as the second argument
    std::string execArg = argv[3];
    if (execArg != "--exec")
    {
        throw std::invalid_argument("--exec should be specified as the second argument");
    }
    args.path = argv[4];

    // read any arguments that the input program accept
    for (int i = MIN_ARG_COUNT; i < argc; ++i)
    {
        args.args.emplace_back(argv[i]);
    }

    return args;
}

int main(int argc, char* argv[])
{
    // Collect input arguments
    Args args{};
    try
    {
        args = parseArgs(argc, argv);
    }
    catch (std::invalid_argument& e)
    {
        std::cerr << e.what() << "\n";
        printHelp();
        std::exit(1);
    }

    // Start debugger
    dbg::Debugger debugger = dbg::Debugger(args.path, args.args, args.var);

    // clang-format off
    debugger.setOnRead(
        [](const dbg::Variable& var)
        {
            std::cout << var.name << "\tread:\t" << var.toString() << "\n";
        });

    debugger.setOnWrite(
        [&debugger](const dbg::Variable& var)
        {
            const auto& lastVar = debugger.getLastVar();
            std::cout << var.name << "\twrite:\t" << lastVar.toString() << " -> " << var.toString() << "\n";
        });
    // clang-format on

    try
    {
        debugger.run();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << "\n";
        printHelp();
        std::exit(2);
    }

    return 0;
}