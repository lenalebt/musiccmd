#include "querycontent.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"

#include "filesystem.hpp"

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
        GaussianMixtureModel<kiss_fft_scalar>* catPositiveTimbreModel = NULL;
        GaussianMixtureModel<kiss_fft_scalar>* catNegativeTimbreModel = NULL;
        GaussianMixtureModel<kiss_fft_scalar>* catPositiveChromaModel = NULL;
        GaussianMixtureModel<kiss_fft_scalar>* catNegativeChromaModel = NULL;
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
            {
                catPositiveTimbreModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getPositiveTimbreModel());
                catNegativeTimbreModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getNegativeTimbreModel());
            }
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
            {
                catPositiveChromaModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getPositiveChromaModel());
                catNegativeChromaModel = GaussianMixtureModel<kiss_fft_scalar>::loadFromJSONString(cat.getCategoryDescription()->getNegativeChromaModel());
            }
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
                    
                    double scoreA = songTimbreModel->compareTo(*catPositiveTimbreModel);
                    double scoreB = catPositiveTimbreModel->compareTo(*songTimbreModel);
                    
                    VERBOSE(0, "positive:" << scoreA);
                    VERBOSE_DB(2, " (built from " << scoreA << " for song-to-model and "
                        << scoreB << " for model-to-song)");
                    VERBOSE(0, std::endl);
                    
                    scoreA = songTimbreModel->compareTo(*catNegativeTimbreModel);
                    scoreB = catNegativeTimbreModel->compareTo(*songTimbreModel);
                    
                    VERBOSE(0, "negative:" << scoreA);
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
                    
                    double scoreA = songChromaModel->compareTo(*catPositiveChromaModel);
                    double scoreB = catPositiveChromaModel->compareTo(*songChromaModel);
                    
                    VERBOSE(0, "positive: " << scoreA);
                    VERBOSE_DB(2, " (built from " << scoreA << " for song-to-model and "
                        << scoreB << " for model-to-song)");
                    VERBOSE(0, std::endl);
                    
                    scoreA = songChromaModel->compareTo(*catNegativeChromaModel);
                    scoreB = catNegativeChromaModel->compareTo(*songChromaModel);
                    
                    VERBOSE(0, "negative: " << scoreA);
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
        
        if (catPositiveTimbreModel)
            delete catPositiveTimbreModel;
        if (catPositiveChromaModel)
            delete catPositiveChromaModel;
        if (catNegativeTimbreModel)
            delete catNegativeTimbreModel;
        if (catNegativeChromaModel)
            delete catNegativeChromaModel;
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

bool export_category(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->export_category)
    {
        if (pOpt->export_categoryParameter.size() < 2)
        {
            VERBOSE(0, "you need to give at least 2 parameters: the first is the name of the category, the second is the file name of the playlist." << std::endl);
            return false;
        }
        else if (pOpt->export_categoryParameter.size() > 3)
        {
            VERBOSE(0, "the maximum number of parameters is 3." << std::endl);
            return false;
        }
        
        std::string categoryName = pOpt->export_categoryParameter[0];
        std::ofstream file(pOpt->export_categoryParameter[1].c_str());
        
        char* realfilename_c = NULL;
        realfilename_c = realpath(pOpt->export_categoryParameter[1].c_str(), NULL);
        std::string playlistFilename(realfilename_c);
        
        int numberOfFiles = 100;
        if (pOpt->export_categoryParameter.size() == 3)
        {
            std::stringstream stream;
            stream << pOpt->export_categoryParameter[2];
            stream >> numberOfFiles;
        }
        
        
        std::vector<std::pair<music::databaseentities::id_datatype, double> > recordingIDsAndScores;
        databaseentities::id_datatype categoryID;
        
        std::vector<databaseentities::id_datatype> categoryIDs;
        conn->getCategoryIDsByName(categoryIDs, categoryName);
        
        if (categoryIDs.size() == 0)
        {   //not found
            VERBOSE(0, "category not found: \"" << categoryName << "\", aborting." << std::endl);
            return false;
        }
        else if (categoryIDs.size() != 1)
        {   //too many
            VERBOSE(0, "found more than one category for search string \"" << categoryName << "\", aborting." << std::endl);
            VERBOSE(0, "found categories:" << std::endl);
            for (std::vector<databaseentities::id_datatype>::const_iterator it = categoryIDs.begin(); it != categoryIDs.end(); ++it)
            {
                databaseentities::Category cat;
                cat.setID(*it);
                conn->getCategoryByID(cat, false);
                VERBOSE(0, "   " << cat.getCategoryName() << std::endl);
            }
            return false;
        }
        categoryID = categoryIDs[0];
        
        std::string playlistPath = playlistFilename.substr(0, playlistFilename.size() - tests::basename(playlistFilename).size());
        
        conn->getRecordingIDsInCategory(recordingIDsAndScores, categoryID, -1.1, 1.1, numberOfFiles);
        for (std::vector<std::pair<music::databaseentities::id_datatype, double> >::iterator it = recordingIDsAndScores.begin(); it != recordingIDsAndScores.end(); ++it)
        {
            databaseentities::Recording rec;
            rec.setID(it->first);
            conn->getRecordingByID(rec, false);
            
            file << (pOpt->export_category_absolute_paths ? rec.getFilename() : toRelativePath(playlistPath, rec.getFilename())) << std::endl;
        }
    }
    
    return true;
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
            if (pOpt->db_verbosity_level >= 1)
            {
                VERBOSE_DB(1, "group members:" << std::endl);
                VERBOSE_DB(1, "  positive: ");
                std::vector<std::pair<databaseentities::id_datatype, double> > recordingIDsAndScores;
                conn->getCategoryExampleRecordingIDs(recordingIDsAndScores, cat.getID(), 0.0, 1.1);
                VERBOSE_DB(1, recordingIDsAndScores.size() << " member(s)" << std::endl);
                for (std::vector<std::pair<databaseentities::id_datatype, double> >::const_iterator it = recordingIDsAndScores.begin(); it != recordingIDsAndScores.end(); ++it)
                {
                    databaseentities::Recording rec;
                    rec.setID(it->first);
                    conn->getRecordingByID(rec, false);
                    VERBOSE_DB(1, "    " << std::left << std::setw(30) << rec.getArtist()
                        << " (" << std::left << std::setw(30) << rec.getAlbum() << ")"
                        << " - " << std::left << std::setw(30) << rec.getTitle() << std::endl);
                }
                recordingIDsAndScores.clear();
                
                VERBOSE_DB(1, "  negative: ");
                conn->getCategoryExampleRecordingIDs(recordingIDsAndScores, cat.getID(), -1.1, 0.0);
                VERBOSE_DB(1, recordingIDsAndScores.size() << " member(s)" << std::endl);
                for (std::vector<std::pair<databaseentities::id_datatype, double> >::const_iterator it = recordingIDsAndScores.begin(); it != recordingIDsAndScores.end(); ++it)
                {
                    databaseentities::Recording rec;
                    rec.setID(it->first);
                    conn->getRecordingByID(rec, false);
                    VERBOSE_DB(1, "    " << std::left << std::setw(30) << rec.getArtist()
                        << " (" << std::left << std::setw(30) << rec.getAlbum() << ")"
                        << " - " << std::left << std::setw(30) << rec.getTitle() << std:: endl);
                }
                recordingIDsAndScores.clear();
                
            }
            if (pOpt->db_verbosity_level >= 2)
            {
                VERBOSE_DB(2, "50 best matches for this category:" << std::endl);
                std::vector<std::pair<databaseentities::id_datatype, double> > recordingIDsAndScores;
                conn->getRecordingIDsInCategory(recordingIDsAndScores, cat.getID(), -1.0, 1000.0, 50);
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
        VERBOSE_DB(3, "\t positive classifier description: " << cat.getCategoryDescription()->getPositiveClassifierDescription() << std::endl);
        VERBOSE_DB(3, "\t negative classifier description: " << cat.getCategoryDescription()->getNegativeClassifierDescription() << std::endl);
        VERBOSE_DB(3, "\t positive timbre model:           " << cat.getCategoryDescription()->getPositiveTimbreModel() << std::endl);
        VERBOSE_DB(3, "\t positive chroma model:           " << cat.getCategoryDescription()->getPositiveChromaModel() << std::endl);
        VERBOSE_DB(3, "\t negative timbre model:           " << cat.getCategoryDescription()->getNegativeTimbreModel() << std::endl);
        VERBOSE_DB(3, "\t negative chroma model:           " << cat.getCategoryDescription()->getNegativeChromaModel() << std::endl);
    }
    else
    {
        VERBOSE_DB(1, "no features found." << std::endl);
    }
}
