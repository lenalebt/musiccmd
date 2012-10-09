#ifndef DB_ADDCONTENT_HPP
#define DB_ADDCONTENT_HPP

#include "music.hpp"

bool addfiles(music::DatabaseConnection* conn, music::FilePreprocessor& proc);
bool addfolders(music::DatabaseConnection* conn, music::FilePreprocessor& proc);

#endif //DB_ADDCONTENT_HPP
