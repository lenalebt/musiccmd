#include "main.hpp"
#include <iostream>
#include <cctype>

#include "music.hpp"

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

#include "addcontent.hpp"
#include "editcontent.hpp"
#include "removecontent.hpp"
#include "querycontent.hpp"

#include "filesystem.hpp"

using namespace std;
using namespace music;

int main(int argc, char *argv[])
{
    VERBOSE(0, "musiccmd - a commandline interface for libmusic" << std::endl);
    
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    srand(time(NULL));
    
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
    
    MultithreadedFilePreprocessor proc(conn, pOpt->timbre_modelsize, pOpt->timbre_dimension, pOpt->timbre_timeslice_size, pOpt->chroma_modelsize, pOpt->chroma_timeslice_size);
    ClassificationProcessor cProc(conn, pOpt->category_timbre_modelsize, pOpt->category_timbre_persong_samplesize, pOpt->category_chroma_modelsize, pOpt->category_chroma_persong_samplesize);
    
    //first add files, then add folders
    if (!add_file(conn, proc, cProc))
    {
        ERROR_OUT("failed to add files. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!add_folder(conn, proc, cProc))
    {
        ERROR_OUT("failed to add folders. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!add_category(conn))
    {
        ERROR_OUT("failed to add category. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!edit_category(conn, cProc))
    {
        ERROR_OUT("failed to edit category. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!recalculate_category(conn, cProc))
    {
        ERROR_OUT("failed to edit category. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!show_category(conn))
    {
        ERROR_OUT("failed to show category. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!export_category(conn))
    {
        ERROR_OUT("failed to export category. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!search_artist_album_title_filename(conn))
    {
        ERROR_OUT("failed to search. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (!clean_db(conn))
    {
        ERROR_OUT("failed to search for filename. aborting.", 0);
        VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
        conn->close();
        return EXIT_FAILURE;
    }
    
    if (pOpt->remove_category)
    {
        ERROR_OUT("removing categories not yet implemented.", 0);
    }
    
    VERBOSE(3, "close database file \"" << pOpt->dbfile << "\"" << std::endl);
    conn->close();
    delete conn;
    
    return EXIT_SUCCESS;
}

int runTest(std::string testname)
{
    std::transform(testname.begin(), testname.end(), testname.begin(), (int (*)(int))std::tolower);
    std::cerr << "running test \"" << testname << "\"..." << std::endl;
    if (testname == "basename")
        return tests::testBasename();
    if (testname == "torelativepath")
        return tests::testToRelativePath();
    else
    {
        std::cerr << "test \"" << testname << "\" not found. aborting." << std::endl;
        return EXIT_FAILURE;
    }
}
