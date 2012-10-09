#include "main.hpp"
#include <iostream>

#include "music.hpp"

#include "programoptions.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    po::variables_map vm;
	if (parseProgramOptions(argc, argv, vm) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    
    //TODO: run adding/querying, etc.
    
    return EXIT_SUCCESS;
}



