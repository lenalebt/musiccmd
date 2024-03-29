#ifndef DB_EDITCONTENT_HPP
#define DB_EDITCONTENT_HPP

#include "music.hpp"

bool edit_category(music::DatabaseConnection* conn, music::ClassificationProcessor& cProc);
bool recalculate_category(music::DatabaseConnection* conn, music::ClassificationProcessor& cProc);
bool copy_category(music::DatabaseConnection* conn);

#endif  //DB_EDITCONTENT_HPP
