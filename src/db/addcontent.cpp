#include "addcontent.hpp"

#include <vector>
#include <string>

#include "programoptions.hpp"
#include "logging.hpp"

bool addfiles(music::DatabaseConnection* conn, music::FilePreprocessor& proc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->addfile)
    {
        VERBOSE(0, "add files to database." << std::endl);
        std::vector<std::string>* files = &pOpt->addfileParameter;
        music::databaseentities::id_datatype recordingID;
        for (std::vector<std::string>::const_iterator it = files->begin(); it != files->end(); it++)
        {
            //check if the file already is in the database...
            if (conn->getRecordingIDByFilename(recordingID, *it) && (recordingID != -1))
            {
                VERBOSE(1, "file \"" << *it << "\" already in database, skipping." << std::endl);
                continue;
            }
            
            VERBOSE(1, "add \"" << *it << "\"" << std::flush);
            proc.preprocessFile(*it, recordingID, conn);
            VERBOSE(2, ", recording ID: " << recordingID);
            VERBOSE(1, std::endl);
        }
    }
    
    return true;
}
bool addfolders(music::DatabaseConnection* conn, music::FilePreprocessor& proc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->addfolder)
    {
        ERROR_OUT("not yet implemented: adding folders", 0);
    }
    
    return true;
}
