#pragma once

#include <sqlite3.h>
#include <stddef.h>

#define TT_MAX_TEXT_LEN 200
#define TT_MAX_IMAGE_LEN 200

typedef struct {
    int id;
    char text[TT_MAX_TEXT_LEN];
    char image[TT_MAX_IMAGE_LEN];
} Tutorial;

typedef struct {
    sqlite3* db;
} TutorialDB;

void tutorial_init(sqlite3* db);
Tutorial* get_tutorial_alloc(size_t id);
