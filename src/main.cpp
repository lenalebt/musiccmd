#include "main.hpp"
#include <iostream>
#include <cctype>

#include "music.hpp"

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

#include "addcontent.hpp"
#include "querycontent.hpp"

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
    
    VERBOSE(3, "open database file \"" << pOpt->dbfile << "\"" << std::endl);
    DatabaseConnection* conn = new SQLiteDatabaseConnection();
    conn->open(pOpt->dbfile);
    
    FilePreprocessor proc;
    
    //TODO: first run adding files/folders
    //first add files, then add folders
    if (!addfiles(conn, proc))
    {
        ERROR_OUT("failed to add files. aborting.", 0);
        return EXIT_FAILURE;
    }
    
    if (!addfolders(conn, proc))
    {
        ERROR_OUT("failed to add folders. aborting.", 0);
        return EXIT_FAILURE;
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
