#include "programoptions.hpp"
#include "debug.hpp"


int parseProgramOptions(int argc, char* argv[], po::variables_map& vm)
{
    po::options_description optAll("All options:");
    
    po::options_description optGlobal("Global options:");
    optGlobal.add_options()
        ("help,h", "Show help message and exit.")
        ("dbfile", po::value<std::string>()->default_value("database.db"),
            "Set database file (where to store results etc.).")
        ("version,v", "Show version number and exit.")
        ;
    
    po::options_description optAddFiles("Options for adding files to the database:");
    optAddFiles.add_options()
        ("addfile,a", po::value<std::vector<std::string> >(), "Add specific files to the database. You may specify more than one file.")
        ("addfolder,A", po::value<std::vector<std::string> >()->composing(), "Add all files from a folder to the database.  You may specify more than one folder.")
        ;
    
    po::options_description optQueryDB("Options for querying the database:");
    optQueryDB.add_options()
        ("verbose-dbinfo,i", po::value<unsigned int>()->implicit_value(0),
            "Set verbosity level for database queries. Higher numbers indicate "
            "more verbose output.")
        ("search-artist", po::value<std::string>(), "Search for all "
            "recordings from an artist. You may use wildcards \"*\" "
            "and \"?\".")
        ("search-title",  po::value<std::string>(), "Search for all "
            "recordings with given title. You may use wildcards \"*\" "
            "and \"?\".")
        ;
    //NOTE: on wildcards: simply replace * by % and ? by _, feed it to the DB.
    
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
    
    return EXIT_SUCCESS;
}
