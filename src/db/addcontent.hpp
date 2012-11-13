#ifndef DB_ADDCONTENT_HPP
#define DB_ADDCONTENT_HPP

#include "music.hpp"

bool add_file(music::DatabaseConnection* conn, music::MultithreadedFilePreprocessor& proc, music::ClassificationProcessor& cProc);
bool add_folder(music::DatabaseConnection* conn, music::MultithreadedFilePreprocessor& proc, music::ClassificationProcessor& cProc);

bool add_category(music::DatabaseConnection* conn);

#endif //DB_ADDCONTENT_HPP
