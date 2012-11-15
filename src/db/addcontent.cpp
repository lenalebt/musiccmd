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

#if 0
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
    
    if (realfilename_c == NULL)
    {
        VERBOSE(0, "file did not exist or read error: \"" << filename << "\", skipping..." << std::endl);
        return 0;
    }
    
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
#endif

bool addFilesToDB(music::DatabaseConnection* conn, music::MultithreadedFilePreprocessor& proc, music::ClassificationProcessor& cProc, std::vector<std::string>& files)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    
    music::OutputStreamCallback osc;
    BlockingQueue<music::databaseentities::id_datatype>* recordingIDQueue;
    //preprocess files...
    recordingIDQueue = proc.preprocessFiles(files, pOpt->threadCount, &osc);
    music::databaseentities::id_datatype recordingID;
    
    VERBOSE(0, "classification processing..." << std::endl);
    
    while (recordingIDQueue->dequeue(recordingID))
    {
        databaseentities::Recording recording;
        recording.setID(recordingID);
        conn->getRecordingByID(recording, true);
        cProc.addRecording(recording);
    }
    
    delete recordingIDQueue;
    
    return true;
}

bool add_file(music::DatabaseConnection* conn, music::MultithreadedFilePreprocessor& proc, music::ClassificationProcessor& cProc)
{
    ProgramOptions* pOpt = ProgramOptions::getInstance();
    if (pOpt->add_file)
    {
        VERBOSE(0, "add files to database." << std::endl);
        std::vector<std::string>* files = &pOpt->add_fileParameter;
        std::sort(files->begin(), files->end());
        if (!addFilesToDB(conn, proc, cProc, *files))
        {
            ERROR_OUT("adding files failed.", 10);
        }
    }
    
    return true;
}
bool add_folder(music::DatabaseConnection* conn, music::MultithreadedFilePreprocessor& proc, music::ClassificationProcessor& cProc)
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
            
            if (dir == NULL)
            {
                VERBOSE(0, "folder did not exist or read error: \"" << folder << "\", skipping..." << std::endl);
                continue;
            }
            
            //load file names and sort them
            while ((ent = readdir(dir)) != NULL)
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
                            
                            //check if the file already is in the database...
                            char* realfilename_c = NULL;
                            realfilename_c = realpath((folder + filename).c_str(), NULL);
                            
                            if (realfilename_c == NULL)
                            {
                                VERBOSE(0, "file did not exist or read error: \"" << (folder + filename) << "\", skipping..." << std::endl);
                                return 0;
                            }
                            
                            std::string realfilename(realfilename_c);
                            free(realfilename_c);
                            music::databaseentities::id_datatype recordingID = -1;
                            if (conn->getRecordingIDByFilename(recordingID, realfilename))
                            {
                                if (recordingID != -1)
                                {
                                    if (pOpt->replace_if_found)
                                    {
                                        VERBOSE(1, "file \"" << realfilename << "\" already in database, replacing." << std::endl);
                                        conn->deleteRecordingByID(recordingID);
                                        files.push_back(folder + filename);
                                    }
                                    else
                                    {
                                        VERBOSE(1, "file \"" << realfilename << "\" already in database, skipping." << std::endl);
                                    }
                                }
                                else
                                    files.push_back(folder + filename);
                            }
                            else
                            {
                                VERBOSE(0, "database error. trying to go on...");
                            }
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
        if (!addFilesToDB(conn, proc, cProc, files))
        {
            ERROR_OUT("adding files failed.", 10);
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
