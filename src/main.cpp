#include "main.hpp"
#include <iostream>
#include <cctype>

#include "music.hpp"

#include "programoptions.hpp"
#include "logging.hpp"

using namespace std;
using namespace music;

int main(int argc, char *argv[])
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    
	if (parseProgramOptions(argc, argv) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    
    //run program tests before doing anything else.
    if (pOpt->test)
    {
        return runTest(pOpt->testParameter);
    }
    
    VERBOSE(3, "open database file \"" << pOpt->dbfile << "\"");
    DatabaseConnection* conn = new SQLiteDatabaseConnection();
    conn->open(pOpt->dbfile);
    
    //TODO: first run adding files/folders
    //first add files, then add folders
    if (pOpt->addfile)
    {
        std::vector<std::string>* files = &pOpt->addfileParameter;
        for (std::vector<std::string>::const_iterator it = files->begin(); it != files->end(); it++)
        {
            std::cerr << *it << std::endl;
        }
    }
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
