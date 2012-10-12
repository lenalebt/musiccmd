#ifndef DB_QUERYCONTENT_HPP
#define DB_QUERYCONTENT_HPP

#include "music.hpp"

bool search_artist_album_title(music::DatabaseConnection* conn);
bool search_filename(music::DatabaseConnection* conn);

void displayRecordingDetails(const music::databaseentities::Recording& rec);

bool show_category(music::DatabaseConnection* conn);

void displayCategoryDetails(const music::databaseentities::Category& cat);

#endif //DB_QUERYCONTENT_HPP
