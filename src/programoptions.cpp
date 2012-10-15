#include "programoptions.hpp"
#include "debug.hpp"
#include "logging.hpp"

#include <algorithm>

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
    
    po::options_description optDatabase("Options for database operations");
    optDatabase.add_options()
        ("add-file,a", po::value<std::vector<std::string> >(&pOpt->add_fileParameter)->composing()->multitoken(),
            "Add specific files to the database. You may specify more "
            "than one file.")
        ("add-folder,A", po::value<std::vector<std::string> >(&pOpt->add_folderParameter)->composing()->multitoken(),
            "Add all files from a folder to the database.  You may specify "
            "more than one folder.")
        ("replace-if-found,x", "Replace files that are found in the database "
            "while adding new files, instead of skipping them.")
        ("clean-db", "Look for files that are no longer available and delete them from the database.")
        ;
    
    po::options_description optClassification("Options for classification");
    optClassification.add_options()
        ("add-category", po::value<std::string>(&pOpt->add_categoryParameter),
            "Add a category with the given name.")
        ("edit-category", po::value<std::vector<std::string> >(&pOpt->edit_categoryParameter)->multitoken(),
            "Edits a category with the given name. You need to specify additional parameters:\n"
            "   add-positive searchparams\n"
            "   add-negative searchparams\n"
            "   remove       searchparams\n"
            "where searchparams are equivalent to the parameters of --search.\n"
            "Example:\n"
            "    \t./musiccmd --edit-category powermetal add-positive artist '*sonata*arctica*'"
            )
        ("recalculate-category", po::value<std::string>(&pOpt->recalculate_categoryParameter),
            "Recalculates the category models. May help if something got wrong"
            " with the category.")
        ("show-category", po::value<std::string>(&pOpt->show_categoryParameter),
            "Show information about a category with the given name. You may use "
            "wildcards \"*\" and \"?\".")
        ("remove-category", po::value<std::string>(&pOpt->remove_categoryParameter),
            "Remove a category with the given name.")   //TODO (delete everything, category-memberships and examples, too!)
        ;
    
    po::options_description optQueryDB("Options for querying the database");
    optQueryDB.add_options()
        ("verbose-dbinfo,i", po::value<unsigned int>(&pOpt->db_verbosity_level)->implicit_value(1)->default_value(0),
            "Set verbosity level for database queries. Higher numbers indicate "
            "more verbose output.")
        ("search", po::value<std::vector<std::string> >(&pOpt->searchParameter)->multitoken(),
            "Search for recordings with specified properties."
            "Properties can be selected via:\n"
            "   title  t\n"
            "   artist a\n"
            "   album  b\n"
            "You may use wildcards \"*\" and \"?\"."
            "The order of the elements is not relevant.\n"
            "Example:\n"
            "  \t./musiccmd --search artist \'*freelance*\' title starring\n"
            "would find the song \"Starring\" by \"Freelance Whales\".")
        ("search-filename",  po::value<std::string>(&pOpt->search_filenameParameter),
            "Search for all file names containing the search string.")
        ("show-timbre-scores", po::value<std::string>(&pOpt->show_timbre_scoresParameter),
            "Show scores of "
            "timbre similarity for category with the given name when "
            "searching recordings.")
        ;
    
    po::variables_map vm;
    optAll.add(optGlobal).add(optDatabase).add(optQueryDB).add(optClassification);
    
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
    if (vm.count("add-file"))
        pOpt->add_file = true;
    if (vm.count("add-folder"))
        pOpt->add_folder = true;
    if (vm.count("replace-if-found"))
        pOpt->replace_if_found = true;
    
    if (vm.count("add-category"))
        pOpt->add_category = true;
    if (vm.count("edit-category"))
        pOpt->edit_category = true;
    if (vm.count("recalculate-category"))
        pOpt->recalculate_category = true;
    if (vm.count("show-category"))
        pOpt->show_category = true;
    if (vm.count("remove-category"))
        pOpt->remove_category = true;
    
    if (vm.count("search"))
        pOpt->search = true;
    if (vm.count("search-filename"))
        pOpt->search_filename = true;
    if (vm.count("show-timbre-scores"))
        pOpt->show_timbre_scores = true;
    if (vm.count("clean-db"))
        pOpt->clean_db = true;
    
    pOpt->replaceAllWildcards();
    
    return EXIT_SUCCESS;
}

ProgramOptions* ProgramOptions::instance = NULL;

ProgramOptions::ProgramOptions() :
    verbosity_level(0),
    db_verbosity_level(0),
    
    dbfile("database.db"),
    
    test(false),
    testParameter(),
    
    add_file(false),
    add_fileParameter(),
    
    add_folder(false),
    add_folderParameter(),
    
    replace_if_found(false),
    
    search(false),
    searchParameter(),
    
    search_filename(false),
    search_filenameParameter(),
    
    show_timbre_scores(false),
    show_timbre_scoresParameter(),
    
    clean_db(false),
    
    add_category(false),
    add_categoryParameter(),
    
    edit_category(false),
    edit_categoryParameter(),
    
    recalculate_category(false),
    recalculate_categoryParameter(),
    
    show_category(false),
    show_categoryParameter(),
    
    remove_category(false),
    remove_categoryParameter()
{
    
}

ProgramOptions* ProgramOptions::getInstance()
{
    return (instance==NULL ? instance = new ProgramOptions() : instance);
}

void ProgramOptions::replaceWildcards(std::string& str)
{
    std::replace(str.begin(), str.end(), '*', '%');
    std::replace(str.begin(), str.end(), '?', '_');
}
void ProgramOptions::replaceAllWildcards()
{
    for (unsigned int i=0; i<searchParameter.size(); i++)
        replaceWildcards(searchParameter[i]);
    for (unsigned int i=0; i<edit_categoryParameter.size(); i++)
        replaceWildcards(edit_categoryParameter[i]);
}
