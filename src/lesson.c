#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lesson.h"
#include "sql.h"

global LessonDB db_lesson = { 0 };

int cb_lesson_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)argc;

    int used_arg_count = 0;

    if (0 == strcmp(col_name[0], "category_id")) {
        int category_id = atoi(argv[used_arg_count++]);
        char* category_name = strdup(argv[used_arg_count++]);
        assert(used_arg_count == argc && "SQL Table Changed!");

        da_push_to_id(db_lesson.categories, char*, category_name, category_id);
    } else if (0 == strcmp(col_name[0], "lesson_id")) {
        int lesson_id = atoi(argv[used_arg_count++]);
        int category_id = argv[used_arg_count] == NULL ? 0 : atoi(argv[used_arg_count]);
        used_arg_count++;
        char* title = strdup(argv[used_arg_count++]);
        char* page_name = strdup(argv[used_arg_count++]);
        int question_count = atoi(argv[used_arg_count++]);
        assert(used_arg_count == argc && "SQL Table Changed!");

        Lesson* new = malloc(sizeof(Lesson) + sizeof(LessonQuestion) * question_count);
        new->id = lesson_id;
        new->title = title;
        new->category_id = category_id;
        new->question_count = question_count;
        new->page_name = page_name;
        new->bt = NULL;
        new->bt_text = NULL;

        da_push_to_id(db_lesson.lessons, Lesson*, new, lesson_id);
    } else if (0 == strcmp(col_name[0], "question_id")) {
        int question_id = atoi(argv[used_arg_count++]);
        (void)question_id;

        int lesson_id = atoi(argv[used_arg_count++]);
        int question_pos = atoi(argv[used_arg_count++]);
        int question_type = atoi(argv[used_arg_count++]);
        char* question = argv[used_arg_count++];
        char* choice1 = argv[used_arg_count++];
        char* choice2 = argv[used_arg_count++];
        char* choice3 = argv[used_arg_count++];
        char* choice4 = argv[used_arg_count++];
        int answer = atoi(argv[used_arg_count++]);
        assert(used_arg_count == argc && "SQL Table Changed!");

        Lesson* new = db_lesson.lessons.items[lesson_id];
        assert(question_pos < (int)new->question_count && "[LESSON] Question out of bounds.");

        new->questions[question_pos].answer = answer;
        new->questions[question_pos].type = question_type;

        strncpy(new->questions[question_pos].question, question, MAX_QUESTION_LEN);

        strncpy(new->questions[question_pos].choice[0], choice1, MAX_CHOICE_LEN);
        strncpy(new->questions[question_pos].choice[1], choice2, MAX_CHOICE_LEN);
        strncpy(new->questions[question_pos].choice[2], choice3, MAX_CHOICE_LEN);
        strncpy(new->questions[question_pos].choice[3], choice4, MAX_CHOICE_LEN);
    } else {
        assert(false && "SQL Table Changed!");
    }

    return 0;
}

void lesson_init(sqlite3* db)
{
    da_init(Lesson*, db_lesson.lessons);
    da_init(char*, db_lesson.categories);

    db_lesson.db = db;

    sql_exec(db_lesson.db, cb_lesson_load, NULL, SQL_GET_LESSONS, NULL);
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
