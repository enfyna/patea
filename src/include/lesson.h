#pragma once

#include <gtk/gtk.h>
#include <sqlite3.h>

#include "utils.h"

#define LS_MAX_QUESTION_LEN 200
#define LS_MAX_CHOICE_LEN 50
#define LS_MAX_IMAGE_LEN 50
#define LS_MAX_LESSON_NAME_LEN 16
#define LS_CHOICE_COUNT 4
#define LS_PREFIX "lesson_"
#define LS_PREFIX_LEN (sizeof LS_PREFIX - 1)

enum QUESTION_TYPE {
    QUESTION_TEST,
    QUESTION_TERMINAL,
};

typedef struct {
    enum QUESTION_TYPE type;
    char question[LS_MAX_QUESTION_LEN];
    char choice[LS_CHOICE_COUNT][LS_MAX_CHOICE_LEN];
    char image[LS_MAX_IMAGE_LEN];
    int answer;
} LessonQuestion;

typedef struct {
    size_t id;
    char name[LS_MAX_LESSON_NAME_LEN];
} LessonCategory;

typedef struct {
    GtkWidget* bt; // UI button

    int id;
    char* title;
    char* page_name;
    size_t question_count;
    int category_id;
} Lesson;

typedef struct {
    sqlite3* db;
    da lessons;
} LessonDB;

typedef struct {
    Lesson* ls;
    LessonQuestion* qs;
    int correct_answers;
    size_t q_pos;
} LessonState;

void lesson_init(sqlite3* db);
da lesson_get_lessons(void);
Lesson* lesson_get_from_id(size_t id);
Lesson* lesson_get_from_name(const char* page_name);
LessonQuestion* lesson_get_question(size_t id, size_t pos);
da lesson_get_categories(void);

void lesson_update_bt_texts(int user_id);
void lesson_set_user_result(int user_id, int lesson_id, int result);
