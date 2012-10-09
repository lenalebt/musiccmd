#include "programoptions.hpp"
#include "debug.hpp"
#include "logging.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int parseProgramOptions(int argc, char* argv[])
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    
    po::options_description optAll("All options");
    
    po::options_description optGlobal("Global options");
    optGlobal.add_options()
        ("help,h", "Show help message and exit.")
        ("dbfile", po::value<std::string>(&pOpt->dbfile)->default_value("database.db"),
            "Set database file (where to store results etc.).")
        ("version", "Show version number and exit.")
        ("test", po::value<std::string>(&pOpt->testParameter), "Run test with given name.")
        ("verbosity,v", po::value<unsigned int>(&pOpt->verbosity_level)
            ->implicit_value(1)->default_value(0), 
            "Set verbosity level. Higher numbers indicate more verbosity.")
        ;
    
    po::options_description optAddFiles("Options for adding files to the database");
    optAddFiles.add_options()
        ("addfile,a", po::value<std::vector<std::string> >(&pOpt->addfileParameter)->composing()->multitoken(),
            "Add specific files to the database. You may specify more "
            "than one file.")
        ("addfolder,A", po::value<std::vector<std::string> >(&pOpt->addfolderParameter)->composing()->multitoken(),
            "Add all files from a folder to the database.  You may specify "
            "more than one folder.")
        ;
    
    po::options_description optQueryDB("Options for querying the database");
    optQueryDB.add_options()
        ("verbose-dbinfo,i", po::value<unsigned int>()->implicit_value(1)->default_value(0),
            "Set verbosity level for database queries. Higher numbers indicate "
            "more verbose output.")
        ("search-artist", po::value<std::string>(&pOpt->searchartistParameter),
            "Search for all recordings from an artist. You may use "
            "wildcards \"*\" and \"?\".")
        ("search-title",  po::value<std::string>(&pOpt->searchtitleParameter),
            "Search for all recordings with given title. You may use "
            "wildcards \"*\" and \"?\".")
        ("search-filename",  po::value<std::string>(&pOpt->searchfilenameParameter),
            "Search for all file names containing the search string. You may use "
            "wildcards \"*\" and \"?\".")
        ;
    //NOTE: on wildcards: simply replace * by % and ? by _, feed it to the DB.
    
    po::variables_map vm;
    optAll.add(optGlobal).add(optAddFiles).add(optQueryDB);
    
    try{po::store(po::parse_command_line(argc, argv, optAll), vm);}
    catch(po::error& e)
    {
        std::cerr << "error while parsing the commandline." << std::endl;
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << optAll << std::endl;
        return EXIT_FAILURE;
    }
    
    //produce version message
    if (vm.count("version")) {
        std::cerr << "musiccmd version " << QUOTEME(VERSION) << std::endl;
        if (!vm.count("help"))
            return EXIT_FAILURE;
    }
    //produce help message
    if (vm.count("help")) {
        std::cerr << "musiccmd is a commandline frontend for libmusic. " <<
            "You may call it with the following options:" << std::endl;
        std::cerr << optAll << std::endl;
        return EXIT_FAILURE;
    }
    
    po::notify(vm);
    
    if (vm.count("test"))
        pOpt->test = true;
    if (vm.count("addfile"))
        pOpt->addfile = true;
    if (vm.count("addfolder"))
        pOpt->addfolder = true;
    if (vm.count("searchartist"))
        pOpt->searchartist = true;
    if (vm.count("searchtitle"))
        pOpt->searchtitle = true;
    if (vm.count("searchfilename"))
        pOpt->searchfilename = true;
    
    return EXIT_SUCCESS;
}

ProgramOptions* ProgramOptions::instance = NULL;

ProgramOptions::ProgramOptions() :
    verbosity_level(0),
    
    dbfile(""),
    
    test(false),
    testParameter(""),
    
    addfile(false),
    addfileParameter(),
    
    addfolder(false),
    addfolderParameter(),
    
    searchartist(false),
    searchartistParameter(""),
    
    searchtitle(false),
    searchtitleParameter(""),
    
    searchfilename(false),
    searchfilenameParameter("")
    
    
{
    
}

ProgramOptions* ProgramOptions::getInstance()
{
    return (instance==NULL ? instance = new ProgramOptions() : instance);
}
