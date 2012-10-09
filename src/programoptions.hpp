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


class ProgramOptions
{
private:
    static ProgramOptions* instance;
    ProgramOptions();
protected:
    
public:
    static ProgramOptions* getInstance();
    
    unsigned int verbosity_level;
    
    std::string dbfile;
    
    bool test;
    std::string testParameter;
    
    //options for adding files
    bool addfile;
    std::vector<std::string> addfileParameter;
    
    bool addfolder;
    std::vector<std::string> addfolderParameter;
    
    //options for database queries
    bool searchartist;
    std::string searchartistParameter;
    
    bool searchtitle;
    std::string searchtitleParameter;
};
#endif      //PROGRAMOPTIONS_HPP
