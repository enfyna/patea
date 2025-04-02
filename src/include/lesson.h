#pragma once

#include <glib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdbool.h>

#define CHOICE_COUNT 4

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
    char* title;
    char* page_name;
    size_t total_questions;
    int category;
    LessonQuestion questions[];
} Lesson;

typedef struct {
    Lesson** lessons;
    int lesson_count;
    int lesson_capacity;
    char** categories;
    int categories_count;
    int categories_capacity;
} LessonDB;

typedef struct {
    Lesson* lesson;
    int correct_answers;
    size_t question;
} LessonState;

void lesson_init(sqlite3* db);
Lesson* lesson_get_from_id(int id);
Lesson* lesson_get_from_name(const char* page_name);
LessonDB* lesson_get_db(void);
