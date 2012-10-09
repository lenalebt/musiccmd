#ifndef DB_ADDCONTENT_HPP
#define DB_ADDCONTENT_HPP

#include "music.hpp"

bool add_file(music::DatabaseConnection* conn, music::FilePreprocessor& proc);
bool add_folder(music::DatabaseConnection* conn, music::FilePreprocessor& proc);

#endif //DB_ADDCONTENT_HPP
