#include "removecontent.hpp"

#include "debug.hpp"
#include "logging.hpp"
#include "querycontent.hpp"

#include <sys/stat.h>

using namespace music;

bool clean_db(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->clean_db)
    {
        VERBOSE(0, "cleaning database (deleting nonexisting files from the database)..." << std::endl);
        
        conn->beginTransaction();
        //read all recording ids...
        std::vector<music::databaseentities::id_datatype> recordingIDs;
        if (!conn->getRecordingIDsByProperties(recordingIDs, "%", "%", "%"))
        {
            conn->rollbackTransaction();
            return false;
        }
        
        for (std::vector<music::databaseentities::id_datatype>::const_iterator recID = recordingIDs.begin(); recID != recordingIDs.end(); recID++)
        {
            databaseentities::Recording rec;
            databaseentities::id_datatype recordingID = *recID;
            rec.setID(recordingID);
            if (!conn->getRecordingByID(rec, true))
            {
                conn->rollbackTransaction();
                return false;
            }
            
            //check if file exists
            struct stat buf;
            if (stat(rec.getFilename().c_str(), &buf) != 0)
            {
                //most probably, file does not exist: remove from database.
                VERBOSE_DB(0, "file does not exist: \"" << rec.getFilename() << "\", removing from database..."
                    << std::endl << "details:" << std::endl);
                displayRecordingDetails(rec);
                
                conn->deleteCategoryExampleScoresByRecordingID(recordingID);
                conn->deleteRecordingToCategoryScoresByRecordingID(recordingID);
                conn->deleteRecordingByID(recordingID);
                //TODO: Remove orphaned artists, albums, ...
                
                VERBOSE_DB(0, std::endl);
            }
            else
            {
                //VERBOSE_DB(0, "file exists: \"" << rec.getFilename() << "\"" << std::endl);
            }
            
        }
        
        conn->endTransaction();
    }
    return true;
}
