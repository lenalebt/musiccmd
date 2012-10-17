#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP

#include <string>
#include <vector>

/**
 * @brief Parses the program options from the command line.
 * 
 * This function parses the program options given on the command line,
 * but it also displays help messages and version info.
 * 
 * @param argc The argument count
 * @param argv The argument values
 * @param vm The variables map of the parsed program options.
 * 
 * @return <code>EXIT_SUCCESS</code> if everything went fine, <code>EXIT_FAILURE</code>
 *      otherwise.
 */
int parseProgramOptions(int argc, char* argv[]);


/**
 * @brief This class holds the program options
 * 
 * It is implemented as a singleton class.
 * 
 * @attention Expect every function to have knowledge about this object.
 *      Some functions don't need extra parameters as they extract it from
 *      here.
 */
class ProgramOptions
{
private:
    static ProgramOptions* instance;
    ProgramOptions();
protected:
    void replaceWildcards(std::string& str);
public:
    static ProgramOptions* getInstance();
    /**
     * @brief Replaces all non-SQL-wildcards by SQL-wildcards
     * 
     * <code>* -> %</code>, <code>? -> _</code>
     * 
     * @return 
     */
    void replaceAllWildcards();
    
    unsigned int verbosity_level;
    unsigned int db_verbosity_level;
    
    std::string dbfile;
    
    bool test;
    std::string testParameter;
    
    //options for adding files
    bool add_file;
    std::vector<std::string> add_fileParameter;
    
    bool add_folder;
    std::vector<std::string> add_folderParameter;
    
    bool replace_if_found;
    
    //options for database queries
    bool search;
    std::vector<std::string> searchParameter;
    
    bool show_timbre_scores;
    std::string show_timbre_scoresParameter;
    
    bool clean_db;
    
    //options for classification
    bool add_category;
    std::string add_categoryParameter;
    
    bool edit_category;
    std::vector<std::string> edit_categoryParameter;
    
    bool recalculate_category;
    std::string recalculate_categoryParameter;
    
    bool show_category;
    std::string show_categoryParameter;
    
    bool remove_category;
    std::string remove_categoryParameter;
    
    //options for feature extraction fine-tuning
    double       timbre_timeslice_size;
    unsigned int timbre_dimension;
    unsigned int timbre_modelsize;
    
    unsigned int category_timbre_modelsize;
    unsigned int category_persong_samplesize;
};
#endif      //PROGRAMOPTIONS_HPP
