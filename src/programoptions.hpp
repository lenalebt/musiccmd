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
    
    //options for database queries
    bool search_artist;
    std::string search_artistParameter;
    
    bool search_album;
    std::string search_albumParameter;
    
    bool search_title;
    std::string search_titleParameter;
    
    bool search_filename;
    std::string search_filenameParameter;
    
    bool clean_db;
};
#endif      //PROGRAMOPTIONS_HPP
