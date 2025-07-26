#include <assert.h>
#include <glib.h>
#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lesson.h"
#include "sql.h"
#include "text.h"
#include "utils.h"

global LessonDB db_lesson = { 0 };

int cb_question_load(void* data, int argc, char** argv, char** col_name)
{
    LessonQuestion** l = data;
    LessonQuestion* p = calloc(1, sizeof(LessonQuestion));
    *l = p;

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "answer")) {
            p->answer = atoi(argv[i]);
            used_arg_count++;
        } else if (starts_with(col_name[i], "choice")) {
            const char* c_idx = col_name[i] + (sizeof "choice" - 1);
            const int idx = atoi(c_idx) - 1;
            strncpy(p->choice[idx], argv[i], sizeof p->choice[idx]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "question")) {
            strncpy(p->question, argv[i], sizeof p->question);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "image")) {
            strncpy(p->image, argv[i], sizeof p->image);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "question_type")) {
            p->type = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 8 && "Question table changed");

    return 0;
}

LessonQuestion* lesson_get_question_alloc(size_t id, size_t pos)
{
    LessonQuestion* l = NULL;

    sql_exec(db_lesson.db, cb_question_load, &l, SQL_GET_LESSON_QUESTION, id, pos);

    return l;
}

int cb_category_load(void* data, int argc, char** argv, char** col_name)
{
    da* lcc = data;
    LessonCategory* new = calloc(1, sizeof(LessonCategory));

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "category_id")) {
            new->id = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "name")) {
            strncpy(new->name, argv[i], sizeof new->name);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 2 && "Category table changed");

    da_push(*lcc, LessonCategory*, new);

    return 0;
}

da lesson_get_categories_alloc(void)
{
    da lcc = {
        .items = malloc(sizeof(LessonCategory*) * 2),
        .count = 0,
        .capacity = 2,
    };

    sql_exec(db_lesson.db, cb_category_load, &lcc, SQL_GET_LESSON_CATEGORIES);

    return lcc;
}

int cb_lesson_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;

    Lesson* new = malloc(sizeof(Lesson));
    new->bt = NULL;

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "question_count")) {
            new->question_count = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "title")) {
            new->title = strdup(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "page_name")) {
            new->page_name = strdup(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "lesson_id")) {
            new->id = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "category_id")) {
            new->category_id = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 5 && "Lesson table changed");

    da_push(db_lesson.lessons, Lesson*, new);

    return 0;
}

void lesson_init(sqlite3* db)
{
    da_init(Lesson*, db_lesson.lessons);

    db_lesson.db = db;

    sql_exec(db_lesson.db, cb_lesson_load, NULL, SQL_GET_LESSONS);
}

da lesson_get_lessons(void)
{
    return db_lesson.lessons;
}

Lesson* lesson_get_from_name(const char* name)
{
    // removing "lesson_" prefix
    const char* lesson_name = name + LS_PREFIX_LEN;

    for (size_t i = 0; i < db_lesson.lessons.count; i++) {
        Lesson* lesson = db_lesson.lessons.items[i];
        if (0 == strcmp(lesson->page_name, lesson_name)) {
            return lesson;
        }
    }

    assert(false && "[LESSON] Unknown lesson_name.\n");
}

Lesson* lesson_get_from_id(size_t id)
{
    for (size_t i = 0; i < db_lesson.lessons.count; i++) {
        Lesson* lesson = db_lesson.lessons.items[i];
        if (lesson->id == (int)id) {
            return lesson;
        }
    }
    assert(false && "[LESSON] Unknown lesson id.\n");
}

void lesson_set_user_result(int user_id, int lesson_id, int result)
{
    sql_exec(db_lesson.db, NULL, NULL, SQL_INSERT_LESSON_RESULT,
        result, user_id, lesson_id);
}

int cb_lesson_result_get(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)col_name;

    int correct_count, lesson_id;

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "correct_count")) {
            correct_count = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "lesson_id")) {
            lesson_id = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 2 && "Lesson Result table changed");

    Lesson* lesson = lesson_get_from_id(lesson_id);
    char text[64];
    snprintf(text, sizeof text, TX_LESSON_BUTTON,
        lesson->title, correct_count / (float)lesson->question_count * 100);
    gtk_button_set_label(GTK_BUTTON(lesson->bt), text);

    return 0;
}

void lesson_update_bt_texts(int user_id)
{
    char text[64];
    for (size_t i = 0; i < db_lesson.lessons.count; i++) {
        Lesson* lesson = db_lesson.lessons.items[i];
        snprintf(text, sizeof text, TX_LESSON_BUTTON, lesson->title, 0.0);
        gtk_button_set_label(GTK_BUTTON(lesson->bt), text);
    }

    sql_exec(db_lesson.db, cb_lesson_result_get, NULL, SQL_GET_USER_RESULTS, user_id);
}

int cb_lesson_daily_result_get(void* data, int argc, char** argv, char** col_name)
{
    UserDailyResult* udr = data;

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "correct")) {
            udr->correct = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "solved")) {
            udr->solved = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 2 && "Lesson Daily Result table changed");

    return 0;
}

UserDailyResult lesson_get_daily_result(size_t user_id)
{
    UserDailyResult udr = { 0 };

    sql_exec(db_lesson.db, cb_lesson_daily_result_get, &udr, SQL_GET_USER_DAILY_RESULT, user_id);

    return udr;
}
