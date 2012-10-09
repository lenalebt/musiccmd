#include "querycontent.hpp"

#include <vector>
#include <string>

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

using namespace music;

bool search_artist(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->search_artist)
    {
        ERROR_OUT("not yet implemented: searching for artist", 0);
    }
    
    return true;
}
bool search_title(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->search_title)
    {
        std::vector<databaseentities::id_datatype> recordingIDs;
        
        conn->getRecordingIDsByProperties(recordingIDs, "%", pOpt->search_titleParameter, "%");
        for (std::vector<databaseentities::id_datatype>::const_iterator it = recordingIDs.begin(); it != recordingIDs.end(); it++)
        {
            databaseentities::Recording rec;
            rec.setID(*it);
            conn->getRecordingByID(rec, true);
            
            VERBOSE(0, "title found: \"" << rec.getTitle() << "\"." << std::endl);
            
            displayRecordingDetails(rec);
        }
    }
    
    return true;
}
bool search_filename(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->search_filename)
    {
        databaseentities::id_datatype recordingID;
        if (conn->getRecordingIDByFilename(recordingID, pOpt->search_filenameParameter) && (recordingID != -1))
        {
            databaseentities::Recording rec;
            rec.setID(recordingID);
            conn->getRecordingByID(rec, true);
            
            VERBOSE(0, "file found: \"" << rec.getFilename() << "\"." << std::endl);
            
            displayRecordingDetails(rec);
        }
        else
        {
            VERBOSE(0, "file not found: \"" << pOpt->search_filenameParameter << "\"." << std::endl);
        }
    }
    
    return true;
}

void displayRecordingDetails(const databaseentities::Recording& rec)
{
    VERBOSE(0, "\t artist:        " << rec.getArtist() << std::endl);
    VERBOSE(0, "\t title:         " << rec.getTitle() << std::endl);
    VERBOSE(0, "\t album:         " << rec.getAlbum() << std::endl);
    VERBOSE(0, "\t filename:      " << rec.getFilename() << std::endl);
    if (rec.getRecordingFeatures() != NULL)
    {
        VERBOSE(1, "\t tempo:         " << rec.getRecordingFeatures()->getTempo() << std::endl);
        VERBOSE(1, "\t dynamic range: " << rec.getRecordingFeatures()->getDynamicRange() << std::endl);
        VERBOSE(2, "\t timbre model:  " << rec.getRecordingFeatures()->getTimbreModel() << std::endl);
    }
    else
    {
        VERBOSE(1, "no features found." << std::endl);
    }
}
