#include "addcontent.hpp"

#include <vector>
#include <string>

#include "programoptions.hpp"
#include "logging.hpp"
#include "debug.hpp"
#include "stringhelper.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <algorithm>


using namespace music;

/**
 * @brief Adds a file to the database.
 * @return <code>1</code> if the operation succeeded, <code>0</code> if not; <code>-1</code>
 *      if the file already is in the database and thus was skipped.
 */
int addFileToDB(music::DatabaseConnection* conn, music::FilePreprocessor& proc, music::ClassificationProcessor& cProc, const std::string& filename)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    
    databaseentities::id_datatype recordingID;
    //check if the file already is in the database...
    char* realfilename_c = NULL;
    realfilename_c = realpath(filename.c_str(), NULL);
    std::string realfilename(realfilename_c);
    free(realfilename_c);
    if (conn->getRecordingIDByFilename(recordingID, realfilename))
    {
        if (recordingID != -1)
        {
            if (pOpt->replace_if_found)
            {
                VERBOSE(1, "file \"" << realfilename << "\" already in database, replacing." << std::endl);
                if (!conn->deleteRecordingByID(recordingID))
                    return 0;
                recordingID = -1;
            }
            else
            {
                VERBOSE(1, "file \"" << realfilename << "\" already in database, skipping." << std::endl);
                return -1;
            }
        }
    }
    else
    {
        return 0;
    }
    
    //preprocess that file...
    VERBOSE(1, "add \"" << realfilename << "\"" << std::flush);
    if (proc.preprocessFile(realfilename, recordingID))
    {
        VERBOSE(2, ", recording ID: " << recordingID);
        
        VERBOSE(2, ", classifying song... ");
        //TODO: calculate classification scores
        databaseentities::Recording recording;
        recording.setID(recordingID);
        conn->getRecordingByID(recording, true);
        cProc.addRecording(recording);
        VERBOSE(2, "done!");
        
        VERBOSE(1, std::endl);
        return 1;
    }
    else
    {
        VERBOSE(1, " - adding failed!" << std::endl);
        return 0;
    }
}

bool add_file(music::DatabaseConnection* conn, music::FilePreprocessor& proc, music::ClassificationProcessor& cProc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->add_file)
    {
        VERBOSE(0, "add files to database." << std::endl);
        std::vector<std::string>* files = &pOpt->add_fileParameter;
        for (unsigned int i=0; i < files->size(); i++)
        {
            VERBOSE(0, std::fixed << std::setprecision(2) << 100.0*double(i)/double(files->size()) << "%, ");
            if (addFileToDB(conn, proc, cProc, (*files)[i]) == 0)
            {
                ERROR_OUT("adding file failed.", 10);
            }
        }
    }
    
    return true;
}
bool add_folder(music::DatabaseConnection* conn, music::FilePreprocessor& proc, music::ClassificationProcessor& cProc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->add_folder)
    {
        VERBOSE(0, "add files from folders to database." << std::endl);
        DIR* dir = NULL;        //POSIX standard calls
        struct dirent *ent;
        std::vector<std::string> files;
        
        //load contents for every folder in the list
        std::vector<std::string>* folders = &pOpt->add_folderParameter;
        for (std::vector<std::string>::size_type i = 0; i < folders->size(); i++)
        {
            std::string folder = (*folders)[i];
            if (!endsWith(folder, "/"))
                folder += "/";
            
            VERBOSE(2, "opening folder \"" << folder << "\"" << std::endl);
            dir = opendir(folder.c_str());
            
            //load file names and sort them
            while ((ent = readdir (dir)) != NULL)
            {
                std::string filename(ent->d_name);
                struct stat buf;
                if (stat((folder + filename).c_str(), &buf) == 0)
                {   //don't do anything if the file does not exist or another error happened
                    if (S_ISREG(buf.st_mode))
                    {
                        std::string loweredfilename = filename;
                        tolower(loweredfilename);
                        
                        if (endsWith(loweredfilename, ".mp3")
                            || endsWith(loweredfilename, ".wav")
                            || endsWith(loweredfilename, ".ogg")
                            || endsWith(loweredfilename, ".flac")
                            //|| endswith(loweredfilename, ".aac")
                            )
                        {
                            files.push_back(folder + filename);
                            //std::cerr << filename << std::endl;
                        }
                        else
                        {
                            VERBOSE(1, "skipping non-supported file type in file \"" << folder+filename << "\"" << std::endl);
                        }
                    }
                    else if (S_ISDIR(buf.st_mode))
                    {
                        if (pOpt->add_recursive_directories && (filename != "..") && (filename != "."))
                        {
                            folders->push_back(folder+filename);
                        }
                        else
                            {VERBOSE(1, "skipping folder \"" << folder+filename << "\"" << std::endl);}
                    }
                    else
                    {
                        VERBOSE(1, "skipping non-regular file \"" << folder+filename << "\"" << std::endl);
                    }
                }
            }
            closedir(dir);
        }
        
        std::sort(files.begin(), files.end());
        for (unsigned int i=0; i < files.size(); i++)
        {
            VERBOSE(0, std::fixed << std::setprecision(2) << 100.0*double(i)/double(files.size()) << "%, ");
            if (addFileToDB(conn, proc, cProc, files[i]) == 0)
            {
                ERROR_OUT("adding file failed.", 10);
            }
        }
    }
    
    return true;
}

bool add_category(music::DatabaseConnection* conn)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->add_category)
    {
        VERBOSE(1, "add category to database");
        VERBOSE(2, ": " << pOpt->add_categoryParameter);
        VERBOSE(1, "." << std::endl);
        
        music::databaseentities::Category cat;
        cat.setID(-1);
        cat.setCategoryName(pOpt->add_categoryParameter);
        
        conn->addCategory(cat);
    }
    
    return true;
}
