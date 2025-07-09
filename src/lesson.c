#include <assert.h>
#include <glib.h>
#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lesson.h"
#include "sql.h"
#include "utils.h"

global LessonDB db_lesson = { 0 };

int cb_question_load(void* data, int argc, char** argv, char** col_name)
{
    LessonQuestion** l = data;
    (*l) = calloc(1, sizeof(LessonQuestion));

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "answer")) {
            (*l)->answer = atoi(argv[i]);
            used_arg_count++;
        } else if (starts_with(col_name[i], "choice")) {
            const char* idx = col_name[i] + 6;
            strncpy((*l)->choice[atoi(idx) - 1], argv[i], MAX_CHOICE_LEN);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "question")) {
            strncpy((*l)->question, argv[i], MAX_QUESTION_LEN);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "image")) {
            strncpy((*l)->image, argv[i], 50);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "question_type")) {
            (*l)->type = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 8 && "Question table changed");

    return 0;
}

LessonQuestion* lesson_get_question(size_t id, size_t pos)
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
            strncpy(new->name, argv[i], 16);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 2 && "Category table changed");

    da_push(*lcc, LessonCategory*, new);

    return 0;
}

da lesson_get_categories(void)
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
    new->bt_text = NULL;

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

    da_push_to_id(db_lesson.lessons, Lesson*, new, new->id);

    return 0;
}

void lesson_init(sqlite3* db)
{
    da_init(Lesson*, db_lesson.lessons);

    db_lesson.db = db;

    sql_exec(db_lesson.db, cb_lesson_load, NULL, SQL_GET_LESSONS);
}

Lesson* lesson_get_from_name(const char* name)
{
    // removing "lesson_" prefix
    const char* lesson_name = name + LESSON_PAGE_PREFIX_LEN;

    da_foreach(lesson, Lesson*, db_lesson.lessons)
    {
        if (0 == strcmp(lesson->page_name, lesson_name)) {
            return lesson;
        }
    }

    assert(false && "[LESSON] Unknown lesson_name.\n");
}

Lesson* lesson_get_from_id(size_t id)
{
    assert(id > 0 && id <= db_lesson.lessons.count && "[LESSON] Out of bounds.");
    return db_lesson.lessons.items[id];
}

LessonDB* lesson_get_db(void)
{
    return &db_lesson;
}
