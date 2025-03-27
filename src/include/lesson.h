#pragma once

#include <stdbool.h>
#include <glib.h>

#define LESSON_COUNT 2
#define TEST_COUNT 2
#define MAX_TEST_QUESTIONS 10

enum QUESTION_TYPE {
    QUESTION_TERMINAL,
    QUESTION_TEST,
};

enum LESSONS {
    TERMINAL_1,
    TEST_1,

    LESSON_NOT_SELECTED,
};

typedef struct {
    enum LESSONS current;
} LessonState;

typedef struct {
    enum QUESTION_TYPE type;
    char question[200];
    char a[50];
    char b[50];
    char c[50];
    char d[50];
    int answer;
} LessonQuestion;

typedef struct {
    LessonQuestion questions[MAX_TEST_QUESTIONS];
    int total_questions;
    int correct_answers;
    int current_question;
    char* title;
} Lesson;

void lesson_init(void);
Lesson* lesson_get_from_enum(enum LESSONS en);
enum LESSONS lesson_get_from_page(const char* page_name);
