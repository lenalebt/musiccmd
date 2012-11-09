#include "programoptions.hpp"
#include "debug.hpp"
#include "logging.hpp"

#include <algorithm>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int parseProgramOptions(int argc, char* argv[])
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    
    po::options_description optAll("All available options");
    po::options_description optHelp("Parameters");
    
    po::options_description optGlobal("Global options");
    optGlobal.add_options()
        ("help,h", po::value<std::string>()->implicit_value("global"),
            "Show help message and exit. Detailed help is available on "
            "topics:\n"
            "  database:\n"
            "     \tShow options for database access\n"
            "  classification:\n"
            "     \tShow options for classification\n"
            "  features:\n"
            "     \tShow options for feature extraction fine-tuning\n"
            "  category:\n"
            "     \tShow options for category creation fine-tuning\n"
            "  global:\n"
            "     \tShow only global options\n"
            "  standard:\n"
            "     \tShow the most commonly used options\n"
            "  all:\n"
            "     \tShow all available options\n")
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
        ("export-category", po::value<std::vector<std::string> >()->multitoken(),
            "Exports the best matches of a category to a playlist file. "
            "You need to supply the name of the category, a filename, and "
            "optionally the number of matches that should be exported, in this order. "
            "Example:\n"
            "\t./musiccmd --export-category classical classical.m3u\n"
            "\t./musiccmd --export-category metal metal.m3u 100\n"
            "If you do not give the number of recordings that should be "
            "saved to the playlist, a default of 100 will be assumed. Allowed file "
            "types for the playlists are m3u and pls.")
        ;
    
    po::options_description optQueryDB("Options for querying the database");
    optQueryDB.add_options()
        ("verbose-dbinfo,i", po::value<unsigned int>(&pOpt->db_verbosity_level)->implicit_value(1)->default_value(0),
            "Set verbosity level for database queries. Higher numbers indicate "
            "more verbose output.")
        ("search", po::value<std::vector<std::string> >(&pOpt->searchParameter)->multitoken(),
            "Search for recordings with specified properties."
            "Properties can be selected via:\n"
            "   title    t\n"
            "   artist   a\n"
            "   album    b\n"
            "   filename f\n"
            "You may use wildcards \"*\" and \"?\"."
            "The order of the elements is not relevant.\n"
            "Example:\n"
            "  \t./musiccmd --search artist \'*freelance*\' title starring\n"
            "would find the song \"Starring\" by \"Freelance Whales\".")
        ("show-timbre-scores", po::value<std::string>(&pOpt->show_timbre_scoresParameter),
            "Show scores of "
            "timbre similarity for category with the given name when "
            "searching recordings.")
        ("show-chroma-scores", po::value<std::string>(&pOpt->show_chroma_scoresParameter),
            "Show scores of "
            "chroma similarity for category with the given name when "
            "searching recordings.")
        ;
    
    po::options_description optFineTuneFeatures("Options for feature extraction fine-tuning");
    optFineTuneFeatures.add_options()
        ("timbre-timeslice-size", po::value<double>(&pOpt->timbre_timeslice_size)->default_value(0.01),
            "Set the size of the time slices for timbre vector extraction, in seconds. "
            "Typical values are in the range of 0.005 to 0.03.")
        ("timbre-dimension", po::value<unsigned int>(&pOpt->timbre_dimension)->default_value(20),
            "Set the dimension of the timbre vectors. Typical values are "
            "in the range of 4 to 40.")
        ("timbre-modelsize", po::value<unsigned int>(&pOpt->timbre_modelsize)->default_value(20),
            "Set the model size of the timbre vectors, i.e. the number "
            "of normal distributions used to model the timbre vectors. "
            "Typical values are in the range of 5 to 50.")
        ("chroma-timeslice-size", po::value<double>(&pOpt->chroma_timeslice_size)->default_value(0.0625),
            "Set the size of the time slices for chroma vector extraction, in seconds. "
            "Typical values are in the range of 0.05 to 0.3.")
        ("chroma-modelsize", po::value<unsigned int>(&pOpt->chroma_modelsize)->default_value(10),
            "Set the model size of the chroma vectors, i.e. the number "
            "of normal distributions used to model the chroma vectors. "
            "Typical values are in the range of 8 to 20. You should set "
            "at least the number of expected triads, e.g. when seeing "
            "chords G, e, C and C7, the number is 3.")
        ;
    
    po::options_description optFineTuneCategoryCreation("Options for category creation fine-tuning");
    optFineTuneCategoryCreation.add_options()
        ("category-timbre-modelsize", po::value<unsigned int>(&pOpt->category_timbre_modelsize)->default_value(60),
            "Set the model size of the timbre model for a group of songs. "
            "Typical values are in the range of 20 to 100.")
        ("category-timbre-persong-samplesize", po::value<unsigned int>(&pOpt->category_timbre_persong_samplesize)->default_value(20000),
            "Set the number of samples drawn from the timbre model for one song to "
            "build the model for a group of songs. "
            "Typical values are in the range of 1000 to 30000.")
        ("category-chroma-modelsize", po::value<unsigned int>(&pOpt->category_chroma_modelsize)->default_value(10),
            "Set the model size of the chroma model for a group of songs. "
            "Typical values are in the range of 8 to 20.")
        ("category-chroma-persong-samplesize", po::value<unsigned int>(&pOpt->category_chroma_persong_samplesize)->default_value(2000),
            "Set the number of samples drawn from the chroma model for one song to "
            "build the model for a group of songs. "
            "Typical values are in the range of 500 to 4000.")
        ;
    
    po::variables_map vm;
    optAll.add(optGlobal).add(optDatabase).add(optQueryDB).add(optClassification)
        .add(optFineTuneFeatures).add(optFineTuneCategoryCreation);
    optHelp.add(optGlobal).add(optDatabase).add(optQueryDB).add(optClassification);
    
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
        if (vm["help"].as<std::string>() == "standard")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optHelp << std::endl;
        }
        else if (vm["help"].as<std::string>() == "global")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optGlobal << std::endl;
        }
        else if (vm["help"].as<std::string>() == "all")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optAll << std::endl;
        }
        else if (vm["help"].as<std::string>() == "features")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optFineTuneFeatures << std::endl;
        }
        else if (vm["help"].as<std::string>() == "category")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optFineTuneCategoryCreation << std::endl;
        }
        else if (vm["help"].as<std::string>() == "database")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            po::options_description optAllDatabase("Database options");
            optAllDatabase.add(optDatabase).add(optQueryDB);
            std::cerr << optAllDatabase << std::endl;
        }
        else if (vm["help"].as<std::string>() == "classification")
        {
            std::cerr << "musiccmd is a commandline frontend for libmusic." << std::endl;
            std::cerr << optClassification << std::endl;
        }
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
    if (vm.count("show-timbre-scores"))
        pOpt->show_timbre_scores = true;
    if (vm.count("show-chroma-scores"))
        pOpt->show_chroma_scores = true;
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
    
    show_timbre_scores(false),
    show_timbre_scoresParameter(),
    
    show_chroma_scores(false),
    show_chroma_scoresParameter(),
    
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
    remove_categoryParameter(),
    
    timbre_timeslice_size(0.01),
    timbre_dimension(20),
    timbre_modelsize(20),
    chroma_timeslice_size(0.0625),
    chroma_modelsize(8),
    
    category_timbre_modelsize(60),
    category_timbre_persong_samplesize(20000),
    category_chroma_modelsize(8),
    category_chroma_persong_samplesize(2000)
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
