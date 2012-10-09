#include "main.hpp"
#include <iostream>
#include <cctype>

#include "music.hpp"

#include "programoptions.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    po::variables_map vm;
	if (parseProgramOptions(argc, argv, vm) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    
    //run program tests before doing anything else.
    if (vm.count("test"))
    {
        return runTest(vm["test"].as<std::string>());
    }
    
    //TODO: first run adding files/folders
    //TODO: then run querying, etc.
    
    return EXIT_SUCCESS;
}

int runTest(std::string testname)
{
    std::transform(testname.begin(), testname.end(), testname.begin(), (int (*)(int))::tolower);
    std::cerr << "running test \"" << testname << "\"..." << std::endl;
    if (testname == "basename")
        return tests::testBasename();
    else
    {
        std::cerr << "test \"" << testname << "\" not found. aborting." << std::endl;
        return EXIT_FAILURE;
    }
}
