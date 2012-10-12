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
    if (pOpt->search)
    {
        std::vector<databaseentities::id_datatype> recordingIDs;
        
        std::string artist; //= pOpt->search_artist ? pOpt->search_artistParameter : "%";
        std::string album;  //= pOpt->search_album  ? pOpt->search_albumParameter : "%";
        std::string title;  //= pOpt->search_title  ? pOpt->search_titleParameter : "%";
        
        if (pOpt->searchParameter.size() % 2 != 0)
        {
            VERBOSE(0, "parameter count for --search needs to be even, is " <<
                pOpt->searchParameter.size() << std::endl);
            return false;
        }
        for (unsigned int i=0; i+1<pOpt->searchParameter.size(); i+=2)
        {   //go steps of 2 and make sure we do not access memory that does not belong to us
            std::string cmdName = pOpt->searchParameter[i];
            std::string cmdParam = pOpt->searchParameter[i+1];
            if (cmdName == "title")
            {
                if (title != "")
                    {VERBOSE(0, "title specified twice, which is not allowed." << std::endl); return false;}
                else
                    title = cmdParam;
            }
            else if (cmdName == "artist")
            {
                if (artist != "")
                    {VERBOSE(0, "artist specified twice, which is not allowed." << std::endl); return false;}
                else
                    artist = cmdParam;
            }
            else if (cmdName == "album")
            {
                if (album != "")
                    {VERBOSE(0, "album specified twice, which is not allowed." << std::endl); return false;}
                else
                    album = cmdParam;
            }
            else
            {
                VERBOSE(0, "unknown command \"" << cmdName << "\" with parameter \"" <<
                    cmdParam << "\"."<< std::endl);
                return false;
            }
        }
        if (title == "")
            title = "%";
        if (artist == "")
            artist = "%";
        if (album == "")
            album = "%";
        
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

bool show_category(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->show_category)
    {
        std::vector< databaseentities::id_datatype > categoryIDs;
        
        conn->getCategoryIDsByName(categoryIDs, pOpt->show_categoryParameter);
        
        for (unsigned int i=0; i<categoryIDs.size(); i++)
        {
            music::databaseentities::Category cat;
            cat.setID(categoryIDs[i]);
            conn->getCategoryByID(cat, true);
            VERBOSE_DB(0, "category found: \"" << cat.getCategoryName() << "\"" << std::endl);
            displayCategoryDetails(cat);
        }
    }
    
    return true;
}

void displayCategoryDetails(const music::databaseentities::Category& cat)
{
    VERBOSE_DB(0, "\t name:          " << cat.getCategoryName() << std::endl);
    VERBOSE_DB(1, "\t some features still missing (members, etc.)" << std::endl);
    if (cat.getCategoryDescription() != NULL)
    {
        VERBOSE_DB(3, "\t classifier description: " << cat.getCategoryDescription()->getClassifierDescription() << std::endl);
        VERBOSE_DB(3, "\t timbre model:           " << cat.getCategoryDescription()->getTimbreModel() << std::endl);
    }
    else
    {
        VERBOSE_DB(1, "no features found." << std::endl);
    }
}
