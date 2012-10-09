#ifndef DB_QUERYCONTENT_HPP
#define DB_QUERYCONTENT_HPP

#include "music.hpp"

bool search_artist(music::DatabaseConnection* conn);
bool search_title(music::DatabaseConnection* conn);
bool search_filename(music::DatabaseConnection* conn);

void displayRecordingDetails(const music::databaseentities::Recording& rec);

#endif //DB_QUERYCONTENT_HPP
