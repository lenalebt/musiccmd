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
        {
            VERBOSE(0, "reading DB failed.");
            return false;
        }
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
            conn->updateCategoryExampleScore(category.getID(), *it, std::sqrt(-1.0));
        conn->endTransaction();
        
        VERBOSE(2, "retrain classifier..." << std::endl);
        
        OutputStreamCallback osc;
        cProc.recalculateCategory(category, true, &osc);
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
            VERBOSE_DB(3, "    new positive timbre model: " << category.getCategoryDescription()->getPositiveTimbreModel());
            VERBOSE_DB(3, "    new negative timbre model: " << category.getCategoryDescription()->getNegativeTimbreModel());
            VERBOSE_DB(3, "    new positive chroma model: " << category.getCategoryDescription()->getPositiveChromaModel());
            VERBOSE_DB(3, "    new negative chroma model: " << category.getCategoryDescription()->getNegativeChromaModel());
        }
    }
    
    return true;
}

bool copy_category(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->copy_category)
    {
        if (pOpt->copy_categoryParameter.size() != 2)
        {
            VERBOSE(0, "you need to give exactly 2 parameters: the first is the "
                << "name of the category to copy from, the second is the name of "
                << "the (new) category to copy to." << std::endl);
            return false;
        }
        
        std::string fromCategoryName = pOpt->copy_categoryParameter[0];
        std::string toCategoryName = pOpt->copy_categoryParameter[1];
        
        std::vector<databaseentities::id_datatype> categoryIDs;
        databaseentities::id_datatype fromCategoryID;
        conn->getCategoryIDsByName(categoryIDs, fromCategoryName);
        
        if (categoryIDs.size() == 0)
        {   //not found
            VERBOSE(0, "category not found: \"" << fromCategoryName << "\", aborting." << std::endl);
            return false;
        }
        else if (categoryIDs.size() != 1)
        {   //too many
            VERBOSE(0, "found more than one category for search string \"" << fromCategoryName << "\", aborting." << std::endl);
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
        fromCategoryID = categoryIDs[0];
        databaseentities::Category fromCategory;
        fromCategory.setID(fromCategoryID);
        conn->getCategoryByID(fromCategory, true);
        
        VERBOSE(0, "category to copy from: \"" << fromCategory.getCategoryName() << "\" (id: " << fromCategoryID << ")" << std::endl);
        
        conn->getCategoryIDsByName(categoryIDs, toCategoryName);
        if (categoryIDs.size() != 0)
        {   //too many
            VERBOSE(0, "found categories with the same name as the new category \"" << toCategoryName << "\", aborting." << std::endl);
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
        
        
        conn->beginTransaction();
        music::databaseentities::Category toCategory;
        toCategory.setID(-1);
        toCategory.setCategoryName(toCategoryName);
        databaseentities::CategoryDescription* fromDesc = fromCategory.getCategoryDescription();
        databaseentities::CategoryDescription* desc = new databaseentities::CategoryDescription();
        desc->setNegativeChromaModel(fromDesc->getNegativeChromaModel());
        desc->setPositiveChromaModel(fromDesc->getPositiveChromaModel());
        desc->setNegativeTimbreModel(fromDesc->getNegativeTimbreModel());
        desc->setPositiveTimbreModel(fromDesc->getPositiveTimbreModel());
        desc->setNegativeClassifierDescription(fromDesc->getNegativeClassifierDescription());
        desc->setPositiveClassifierDescription(fromDesc->getPositiveClassifierDescription());
        toCategory.setCategoryDescription(desc);
        conn->addCategory(toCategory);
        
        //copy example scores (limited to 10000 examples for now)
        std::vector<std::pair< databaseentities::id_datatype, double> > scoresAndIDs;
        conn->getCategoryExampleRecordingIDs(scoresAndIDs, fromCategoryID, -1.1, 1.1, 10000);
        for (std::vector<std::pair< databaseentities::id_datatype, double> >::const_iterator it=scoresAndIDs.begin(); it != scoresAndIDs.end(); ++it)
        {
            conn->updateCategoryExampleScore(toCategory.getID(), it->first, it->second);
        }
        
        //copy song scores (limited to 1000000 songs for now)
        std::vector<databaseentities::id_datatype> recordingIDs;
        conn->getRecordingIDs(recordingIDs, 0, 1000000);
        for (std::vector<databaseentities::id_datatype>::const_iterator it=recordingIDs.begin(); it != recordingIDs.end(); ++it)
        {
            double score=0.0;
            //read score...
            conn->getRecordingToCategoryScore(*it, fromCategory.getID(), score);
            //write score...
            conn->updateRecordingToCategoryScore(*it, toCategory.getID(), score);
        }
        
        conn->endTransaction();
    }
    return true;
}
