#pragma once

#include <stdbool.h>
#include <glib.h>

#define LESSON_COUNT 2
#define CHOICE_COUNT 4

enum QUESTION_TYPE {
    QUESTION_TERMINAL,
    QUESTION_TEST,
};

enum LESSONS {
    TERMINAL_1,
    TEST_1,

    LESSON_NOT_SELECTED,
};

enum LESSON_CATEGORIES {
    CATEGORY_TERMINAL,
    CATEGORY_PARDUS,
    CATEGORY_LINUX,
};

typedef struct {
    enum LESSONS current;
} LessonState;

typedef struct {
    enum QUESTION_TYPE type;
    char question[200];
    char choice[4][50];
    int answer;
} LessonQuestion;

typedef struct {
    char* title;
    int correct_answers;
    size_t total_questions;
    size_t current_question;
    enum LESSON_CATEGORIES category;
    LessonQuestion questions[];
} Lesson;

void lesson_init(void);
Lesson* lesson_get_from_enum(enum LESSONS en);
enum LESSONS lesson_get_from_name(const char* page_name);
