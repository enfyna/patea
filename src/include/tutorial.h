#include <sqlite3.h>
#include <stddef.h>

#include "lesson.h"

typedef struct {
    int id;
    char text[200];
    char image[200];
} Tutorial;

typedef struct {
    sqlite3* db;
    da tutorials;
} TutorialDB;

void tutorial_init(sqlite3* db);
Tutorial* get_tutorial(size_t id);
void tutorial_set_user_completed(void);
