#include "querycontent.hpp"

#include <vector>
#include <string>

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

using namespace music;

bool search_artist_album_title_filename(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->search)
    {
        std::vector<databaseentities::id_datatype> recordingIDs;
        
        std::string artist; //= pOpt->search_artist ? pOpt->search_artistParameter : "%";
        std::string album;  //= pOpt->search_album  ? pOpt->search_albumParameter : "%";
        std::string title;  //= pOpt->search_title  ? pOpt->search_titleParameter : "%";
        std::string filename;  //= pOpt->search_title  ? pOpt->search_titleParameter : "%";
        
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
            else if (cmdName == "filename")
            {
                if (filename != "")
                    {VERBOSE(0, "filename specified twice, which is not allowed." << std::endl); return false;}
                else
                    filename = cmdParam;
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
        if (filename == "")
            filename = "%";
        
        VERBOSE(1, "searching for recording, title=\"" << title << "\", artist=\""
            << artist << "\", album=\"" << album << "\", filename=\"" << filename << "\"" << std::endl);
        
        databaseentities::Category cat;
        GaussianMixtureModel<kiss_fft_scalar>* catTimbreModel = NULL;
        GaussianMixtureModel<kiss_fft_scalar>* catChromaModel = NULL;
        if (pOpt->show_timbre_scores)
        {
            VERBOSE(2, "loading timbre model for category " << pOpt->show_timbre_scoresParameter << "." << std::endl);
            
            std::vector<databaseentities::id_datatype> categoryIDs;
            conn->getCategoryIDsByName(categoryIDs, pOpt->show_timbre_scoresParameter);
            
            if (categoryIDs.size() == 0)
            {   //not found
                VERBOSE(0, "category not found: \"" << pOpt->show_timbre_scoresParameter << "\", aborting." << std::endl);
                return false;
            }
            else if (categoryIDs.size() != 1)
            {   //too many
                VERBOSE(0, "found more than one category for search string \"" << pOpt->show_timbre_scoresParameter << "\", aborting." << std::endl);
                VERBOSE(0, "found categories:" << std::endl);
                for (std::vector<databaseentities::id_datatype>::const_iterator it = categoryIDs.begin(); it != categoryIDs.end(); ++it)
                {
                    conn->getCategoryByID(cat, *it);
                    VERBOSE(0, "   " << cat.getCategoryName());
                }
                return false;
            }
            cat.setID(categoryIDs[0]);
            
            conn->getCategoryByID(cat, true);
            if (cat.getCategoryDescription() != NULL)
                catTimbreModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getTimbreModel());
            else
            {
                VERBOSE(0, "chosen category has no category description, which may not happen." << std::endl;)
                return false;
            }
        }
        if (pOpt->show_chroma_scores)
        {
            VERBOSE(2, "loading chroma model for category " << pOpt->show_chroma_scoresParameter << "." << std::endl);
            
            std::vector<databaseentities::id_datatype> categoryIDs;
            conn->getCategoryIDsByName(categoryIDs, pOpt->show_chroma_scoresParameter);
            
            if (categoryIDs.size() == 0)
            {   //not found
                VERBOSE(0, "category not found: \"" << pOpt->show_chroma_scoresParameter << "\", aborting." << std::endl);
                return false;
            }
            else if (categoryIDs.size() != 1)
            {   //too many
                VERBOSE(0, "found more than one category for search string \"" << pOpt->show_chroma_scoresParameter << "\", aborting." << std::endl);
                VERBOSE(0, "found categories:" << std::endl);
                for (std::vector<databaseentities::id_datatype>::const_iterator it = categoryIDs.begin(); it != categoryIDs.end(); ++it)
                {
                    conn->getCategoryByID(cat, *it);
                    VERBOSE(0, "   " << cat.getCategoryName());
                }
                return false;
            }
            cat.setID(categoryIDs[0]);
            
            conn->getCategoryByID(cat, true);
            if (cat.getCategoryDescription() != NULL)
                catChromaModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getChromaModel());
            else
            {
                VERBOSE(0, "chosen category has no category description, which may not happen." << std::endl;)
                return false;
            }
        }
        
        conn->getRecordingIDsByProperties(recordingIDs, artist, title, album, filename);
        VERBOSE(1, "found " << recordingIDs.size() << " recordings:" << std::endl);
        for (std::vector<databaseentities::id_datatype>::const_iterator it = recordingIDs.begin(); it != recordingIDs.end(); it++)
        {
            databaseentities::Recording rec;
            rec.setID(*it);
            conn->getRecordingByID(rec, true);
            
            VERBOSE(0, "title found: \"" << rec.getTitle() << "\"." << std::endl);
            
            displayRecordingDetails(rec);
            if (pOpt->show_timbre_scores)
            {
                VERBOSE(0, "\t timbre score: \t");
                if (rec.getRecordingFeatures() != NULL)
                {
                    GaussianMixtureModel<kiss_fft_scalar>* songTimbreModel = NULL;
                    songTimbreModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(rec.getRecordingFeatures()->getTimbreModel());
                    
                    double scoreA = songTimbreModel->compareTo(*catTimbreModel);
                    double scoreB = catTimbreModel->compareTo(*songTimbreModel);
                    
                    VERBOSE(0, scoreA);
                    VERBOSE_DB(2, " (built from " << scoreA << " for song-to-model and "
                        << scoreB << " for model-to-song)");
                    VERBOSE(0, std::endl);
                    
                    if (songTimbreModel)
                        delete songTimbreModel;
                }
                else
                {
                    VERBOSE(0, "no recording features found, so no score calculated." << std::endl);
                }
            }
            if (pOpt->show_chroma_scores)
            {
                VERBOSE(0, "\t chroma score: \t");
                if (rec.getRecordingFeatures() != NULL)
                {
                    GaussianMixtureModel<kiss_fft_scalar>* songChromaModel = NULL;
                    songChromaModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(rec.getRecordingFeatures()->getChromaModel());
                    
                    double scoreA = songChromaModel->compareTo(*catChromaModel);
                    double scoreB = catChromaModel->compareTo(*songChromaModel);
                    
                    VERBOSE(0, scoreA);
                    VERBOSE_DB(2, " (built from " << scoreA << " for song-to-model and "
                        << scoreB << " for model-to-song)");
                    VERBOSE(0, std::endl);
                    
                    if (songChromaModel)
                        delete songChromaModel;
                }
                else
                {
                    VERBOSE(0, "no recording features found, so no score calculated." << std::endl);
                }
            }
        }
        
        if (catTimbreModel)
            delete catTimbreModel;
        if (catChromaModel)
            delete catChromaModel;
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
        VERBOSE_DB(3, "\t chroma model:  " << rec.getRecordingFeatures()->getChromaModel() << std::endl);
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
        std::vector<databaseentities::id_datatype> categoryIDs;
        
        conn->getCategoryIDsByName(categoryIDs, pOpt->show_categoryParameter);
        
        for (unsigned int i=0; i<categoryIDs.size(); i++)
        {
            music::databaseentities::Category cat;
            cat.setID(categoryIDs[i]);
            conn->getCategoryByID(cat, true);
            VERBOSE_DB(0, "category found: \"" << cat.getCategoryName() << "\"" << std::endl);
            displayCategoryDetails(cat);
            if (pOpt->db_verbosity_level >= 2)
            {
                VERBOSE_DB(2, "50 best matches for this category:" << std::endl);
                std::vector<std::pair<databaseentities::id_datatype, double> > recordingIDsAndScores;
                conn->getRecordingIDsInCategory(recordingIDsAndScores, cat.getID(), 0.0, 1000.0, 5000);
                for (std::vector<std::pair<databaseentities::id_datatype, double> >::const_iterator it = recordingIDsAndScores.begin(); it != recordingIDsAndScores.end(); ++it)
                {
                    databaseentities::Recording rec;
                    rec.setID(it->first);
                    conn->getRecordingByID(rec, false); //do not need features
                    VERBOSE_DB(2, "    " << std::left << std::setw(30) << rec.getArtist()
                        << " (" << std::left << std::setw(30) << rec.getAlbum() << ")"
                        << " - " << std::left << std::setw(30) << rec.getTitle() << ", score "
                        << it->second << std::endl);
                }
            }
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
        VERBOSE_DB(3, "\t chroma model:           " << cat.getCategoryDescription()->getChromaModel() << std::endl);
    }
    else
    {
        VERBOSE_DB(1, "no features found." << std::endl);
    }
}
