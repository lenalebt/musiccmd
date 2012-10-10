#include "querycontent.hpp"

#include <vector>
#include <string>

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

using namespace music;

bool search_artist_album_title(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->search_title || pOpt->search_artist || pOpt->search_album)
    {
        std::vector<databaseentities::id_datatype> recordingIDs;
        
        std::string artist = pOpt->search_artist ? pOpt->search_artistParameter : "%";
        std::string album  = pOpt->search_album  ? pOpt->search_albumParameter : "%";
        std::string title  = pOpt->search_title  ? pOpt->search_titleParameter : "%";
        
        conn->getRecordingIDsByProperties(recordingIDs, artist, title, album);
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
    VERBOSE_DB(0, "\t artist:        " << rec.getArtist() << std::endl);
    VERBOSE_DB(0, "\t title:         " << rec.getTitle() << std::endl);
    VERBOSE_DB(0, "\t album:         " << rec.getAlbum() << std::endl);
    VERBOSE_DB(1, "\t track:         " << rec.getTrackNumber() << std::endl);
    VERBOSE_DB(1, "\t filename:      " << rec.getFilename() << std::endl);
    if (rec.getRecordingFeatures() != NULL)
    {
        VERBOSE_DB(1, "\t length:        " << rec.getRecordingFeatures()->getLength() << std::endl);
        VERBOSE_DB(2, "\t tempo:         " << rec.getRecordingFeatures()->getTempo() << std::endl);
        VERBOSE_DB(2, "\t dynamic range: " << rec.getRecordingFeatures()->getDynamicRange() << std::endl);
        VERBOSE_DB(3, "\t timbre model:  " << rec.getRecordingFeatures()->getTimbreModel() << std::endl);
    }
    else
    {
        VERBOSE_DB(1, "no features found." << std::endl);
    }
}
