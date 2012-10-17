#include "editcontent.hpp"

#include "logging.hpp"
#include "programoptions.hpp"
#include "music.hpp"

using namespace music;

bool edit_category(DatabaseConnection* conn, music::ClassificationProcessor& cProc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->edit_category)
    {
        //TODO
        std::vector<databaseentities::id_datatype> posRecordingIDs;
        std::vector<databaseentities::id_datatype> negRecordingIDs;
        std::vector<databaseentities::id_datatype> remRecordingIDs;
        
        std::string artist; //= pOpt->search_artist ? pOpt->search_artistParameter : "%";
        std::string album;  //= pOpt->search_album  ? pOpt->search_albumParameter : "%";
        std::string title;  //= pOpt->search_title  ? pOpt->search_titleParameter : "%";
        std::string filename;  //= pOpt->search_title  ? pOpt->search_titleParameter : "%";
        
        std::string categoryName;
        
        unsigned int parsePos=2;
        std::string parentCmd;
        std::string newParentCmd;
        
        if (pOpt->edit_categoryParameter.size() < 4)
        {
            VERBOSE(0, "--edit-category needs at least 4 parameters, " <<
                pOpt->edit_categoryParameter.size() << " given." << std::endl);
            return false;
        }
        
        categoryName = pOpt->edit_categoryParameter[0];
        newParentCmd = pOpt->edit_categoryParameter[1];
        if (!((newParentCmd == "add-positive") || (newParentCmd == "add-negative") || (newParentCmd == "remove")))
        {
            VERBOSE(0, "unknown command \"" << newParentCmd << "\"."<< std::endl);
            return false;
        }
        
        while (parsePos < pOpt->edit_categoryParameter.size())
        {
            title = "";
            album = "";
            artist = "";
            filename = "";
            parentCmd = newParentCmd;
            
            unsigned int i;
            for (i=parsePos; i+1<pOpt->edit_categoryParameter.size(); i+=2)
            {   //go steps of 2 and make sure we do not access memory that does not belong to us
                std::string cmdName = pOpt->edit_categoryParameter[i];
                std::string cmdParam = pOpt->edit_categoryParameter[i+1];
                
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
                else if ((cmdName == "add-positive") || (cmdName == "add-negative") || (cmdName == "remove"))
                {
                    newParentCmd = cmdName;
                    parsePos = i+1;
                    break;
                }
                else
                {
                    VERBOSE(0, "unknown command \"" << cmdName << "\" with parameter \"" <<
                        cmdParam << "\"."<< std::endl);
                    return false;
                }
            }
            if (parsePos < i)
                parsePos = i;
            
            if (i==pOpt->edit_categoryParameter.size()-1)
            {
                VERBOSE(0, "parameter count for command \"" << parentCmd
                    << "\" needs to be even." << std::endl);
                return false;
            }
            
            if (title == "")
                title = "%";
            if (artist == "")
                artist = "%";
            if (album == "")
                album = "%";
            if (filename == "")
                filename = "%";
            
            std::vector<databaseentities::id_datatype> tmpRecordingIDs;
            conn->getRecordingIDsByProperties(tmpRecordingIDs, artist, title, album, filename);
            if (parentCmd == "add-positive")
            {
                for (std::vector<databaseentities::id_datatype>::const_iterator it = tmpRecordingIDs.begin(); it != tmpRecordingIDs.end(); ++it)
                    posRecordingIDs.push_back(*it);
            }
            else if (parentCmd == "add-negative")
            {
                for (std::vector<databaseentities::id_datatype>::const_iterator it = tmpRecordingIDs.begin(); it != tmpRecordingIDs.end(); ++it)
                    negRecordingIDs.push_back(*it);
            }
            else if (parentCmd == "remove")
            {
                for (std::vector<databaseentities::id_datatype>::const_iterator it = tmpRecordingIDs.begin(); it != tmpRecordingIDs.end(); ++it)
                    remRecordingIDs.push_back(*it);
            }
            tmpRecordingIDs.clear();
        }
        
        databaseentities::Category category;
        std::vector<databaseentities::id_datatype> categoryIDs;
        if (!conn->getCategoryIDsByName(categoryIDs, categoryName))
            return false;
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
                category.setID(*it);
                conn->getCategoryByID(category, true);
                VERBOSE(0, "   " << category.getCategoryName() << std::endl);
            }
            return false;
        }
        
        category.setID(categoryIDs[0]);
        if (!conn->getCategoryByID(category, true))
            return false;
        
        VERBOSE(1, "editing category \"" << category.getCategoryName() << "\"..." << std::endl);
        
        conn->beginTransaction();
        for (std::vector<databaseentities::id_datatype>::const_iterator it = posRecordingIDs.begin(); it != posRecordingIDs.end(); ++it)
            conn->updateCategoryExampleScore(category.getID(), *it, 1.0);	
        for (std::vector<databaseentities::id_datatype>::const_iterator it = negRecordingIDs.begin(); it != negRecordingIDs.end(); ++it)
            conn->updateCategoryExampleScore(category.getID(), *it, -1.0);
        for (std::vector<databaseentities::id_datatype>::const_iterator it = remRecordingIDs.begin(); it != remRecordingIDs.end(); ++it)
            conn->updateCategoryExampleScore(category.getID(), *it, sqrt(-1.0));
        conn->endTransaction();
        
        VERBOSE(2, "retrain classifier..." << std::endl);
        
        OutputStreamCallback osc;
        cProc.recalculateCategory(category, true, &osc);
        
        //proc
        
    }
    
    return true;
}

bool recalculate_category(music::DatabaseConnection* conn, music::ClassificationProcessor& cProc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->recalculate_category)
    {
        VERBOSE(1, "recalculate category ");
        
        databaseentities::Category category;
        std::vector<databaseentities::id_datatype> categoryIDs;
        if (!conn->getCategoryIDsByName(categoryIDs, pOpt->recalculate_categoryParameter))
            return false;
        if (categoryIDs.size() == 0)
        {   //not found
            VERBOSE(0, "category not found: \"" << pOpt->recalculate_categoryParameter << "\", aborting." << std::endl);
            return false;
        }
        else if (categoryIDs.size() != 1)
        {   //too many
            VERBOSE(0, "found more than one category for search string \"" << pOpt->recalculate_categoryParameter << "\", aborting." << std::endl);
            VERBOSE(0, "found categories:" << std::endl);
            for (std::vector<databaseentities::id_datatype>::const_iterator it = categoryIDs.begin(); it != categoryIDs.end(); ++it)
            {
                category.setID(*it);
                conn->getCategoryByID(category, true);
                VERBOSE(0, "   " << category.getCategoryName() << std::endl);
            }
            return false;
        }
        
        category.setID(categoryIDs[0]);
        if (!conn->getCategoryByID(category, true))
            return false;
        
        VERBOSE(1, category.getCategoryName() << "..." << std::endl);
        
        OutputStreamCallback osc;
        cProc.recalculateCategory(category, true, &osc);
        
        if (category.getCategoryDescription() != NULL)
        {
            VERBOSE_DB(3, "    new timbre model: " << category.getCategoryDescription()->getTimbreModel());
        }
    }
    
    return true;
}
