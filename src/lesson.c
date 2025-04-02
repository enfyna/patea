#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lesson.h"

LessonDB db_lesson = { 0 };

int cb_lesson_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)argc;

    if (0 == strcmp(col_name[0], "category_id")) {
        int category_id = atoi(argv[0]);
        char* category_name = strdup(argv[1]);

        if (db_lesson.categories_capacity < category_id) {
            db_lesson.categories_capacity *= 2;
            db_lesson.categories = realloc(db_lesson.categories, sizeof(char*) * db_lesson.categories_capacity);
        }

        db_lesson.categories[category_id] = category_name;
        db_lesson.categories_count++;

    } else if (0 == strcmp(col_name[0], "lesson_id")) {
        int lesson_id = atoi(argv[0]);
        char* title = strdup(argv[2]);
        int category_id = atoi(argv[1]);
        int question_count = atoi(argv[4]);

        char* page_name = strdup(argv[3]);

        Lesson* new = malloc(sizeof(Lesson) + sizeof(LessonQuestion) * question_count);
        new->title = title;
        new->category = category_id;
        new->total_questions = question_count;
        new->page_name = page_name;

        if (db_lesson.lesson_capacity < lesson_id) {
            db_lesson.lesson_capacity *= 2;
            db_lesson.lessons = realloc(db_lesson.lessons, sizeof(Lesson*) * db_lesson.lesson_capacity);
        }

        db_lesson.lessons[lesson_id] = new;
        db_lesson.lesson_count++;

    } else if (0 == strcmp(col_name[0], "question_id")) {
        int question_id = atoi(argv[0]);
        (void)question_id;

        int lesson_id = atoi(argv[1]);
        int question_pos = atoi(argv[2]);
        int question_type = atoi(argv[3]);
        char* question = argv[4];
        char* choice1 = argv[5];
        char* choice2 = argv[6];
        char* choice3 = argv[7];
        char* choice4 = argv[8];
        int answer = atoi(argv[9]);

        Lesson* new = db_lesson.lessons[lesson_id];
        assert(question_pos < (int)new->total_questions && "[LESSON] Question out of bounds.");

        new->questions[question_pos].answer = answer;
        new->questions[question_pos].type = question_type;

        strcpy(new->questions[question_pos].question, question);

        strcpy(new->questions[question_pos].choice[0], choice1);
        strcpy(new->questions[question_pos].choice[1], choice2);
        strcpy(new->questions[question_pos].choice[2], choice3);
        strcpy(new->questions[question_pos].choice[3], choice4);
    }

    return 0;
}

void lesson_init(sqlite3* db)
{
    db_lesson.categories_capacity = 10;
    db_lesson.categories = malloc(sizeof(char*) * db_lesson.categories_capacity);
    db_lesson.lesson_capacity = 10;
    db_lesson.lessons = malloc(sizeof(Lesson*) * db_lesson.lesson_capacity);

    const char* query = "SELECT * FROM LESSON_CATEGORIES;"
                        "SELECT * FROM LESSONS;"
                        "SELECT * FROM LESSON_QUESTIONS;";
    char* err;
    int rc = sqlite3_exec(db, query, cb_lesson_load, NULL, &err);
    if (rc == SQLITE_OK) {
        g_print("[DB] Lessons read successfully!\n");
    } else {
        g_error("[DB] Error(%d): %s\n", rc, err);
    }
}

Lesson* lesson_get_from_name(const char* name)
{
    // removing "lesson_" prefix
    const char* lesson_name = name + 7;

    for (int i = 1; i <= db_lesson.lesson_count; i++) {
        if (0 == strcmp(db_lesson.lessons[i]->page_name, lesson_name)) {
            return db_lesson.lessons[i];
        }
    }

    return NULL;
}

Lesson* lesson_get_from_id(int id)
{
    assert(id < db_lesson.lesson_count && "[LESSON] Out of bounds.");
    return db_lesson.lessons[id];
}

LessonDB* lesson_get_db(void)
{
    return &db_lesson;
}
