#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "func.h"
#include "lesson.h"
#include "sql.h"

global LessonDB db_lesson = { 0 };

int cb_lesson_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)argc;

    if (0 == strcmp(col_name[0], "user_id")) {
        int user_id = atoi(argv[0]);
        char* user_name = strdup(argv[1]);

        User* new = malloc(sizeof(User));
        new->id = user_id;
        new->name = user_name;

        da_push_to_id(db_lesson.users, User*, new, user_id);
    } else if (0 == strcmp(col_name[0], "category_id")) {
        int category_id = atoi(argv[0]);
        char* category_name = strdup(argv[1]);

        da_push_to_id(db_lesson.categories, char*, category_name, category_id);
    } else if (0 == strcmp(col_name[0], "lesson_id")) {
        int lesson_id = atoi(argv[0]);
        char* title = strdup(argv[2]);
        int category_id = argv[1] == NULL ? 0 : atoi(argv[1]);
        int question_count = atoi(argv[4]);

        char* page_name = strdup(argv[3]);

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

        Lesson* new = db_lesson.lessons.items[lesson_id];
        assert(question_pos < (int)new->question_count && "[LESSON] Question out of bounds.");

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
    da_init(User*, db_lesson.users);
    da_init(Lesson*, db_lesson.lessons);
    da_init(char*, db_lesson.categories);

    sql_exec(cb_lesson_load, SQL_GET_LESSONS, NULL);
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
