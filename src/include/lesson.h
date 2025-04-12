#pragma once

#include <assert.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "da.h"

#define CHOICE_COUNT 4
#define LESSON_PAGE_PREFIX "lesson_"
#define LESSON_PAGE_PREFIX_LEN 7

enum QUESTION_TYPE {
    QUESTION_TEST,
    QUESTION_TERMINAL,
};

typedef struct {
    enum QUESTION_TYPE type;
    char question[200];
    char choice[CHOICE_COUNT][50];
    int answer;
} LessonQuestion;

typedef struct {
    GtkWidget* bt; // UI button
    char* bt_text; // UI button text

    int id;
    char* title;
    char* page_name;
    size_t question_count;
    int category_id;
    LessonQuestion questions[];
} Lesson;

typedef struct {
    int id;
    char* name;
} User;

typedef struct {
    int user_id;
    da lessons;
    da categories;
    da users;
} LessonDB;

typedef struct {
    Lesson* lesson;
    int correct_answers;
    size_t question;
} LessonState;

void lesson_init(sqlite3* db);
Lesson* lesson_get_from_id(size_t id);
Lesson* lesson_get_from_name(const char* page_name);
LessonDB* lesson_get_db(void);
