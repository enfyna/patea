#include <assert.h>
#include <stdio.h>

#include "sql.h"
#include "tutorial.h"
#include "user.h"

TutorialDB db_tutorial = { 0 };

int cb_tutorial_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)argc;

    int used_arg_count = 0;

    if (0 == strcmp(col_name[0], "id")) {
        size_t id = atoi(argv[used_arg_count++]);
        char* text = (argv[used_arg_count++]);
        char* image = (argv[used_arg_count++]);
        assert(used_arg_count == argc && "SQL Table Changed!");

        Tutorial* new = malloc(sizeof(Tutorial));
        new->id = id;
        memcpy(new->text, text, 200);
        memcpy(new->image, image, 200);

        da_push_to_id(db_tutorial.tutorials, Tutorial*, new, id);
    } else {
        assert(false && "SQL Table Changed!");
    }

    return 0;
}

void tutorial_init(sqlite3* db)
{
    db_tutorial.db = db;
    da_init(Tutorial, db_tutorial.tutorials);

    sql_exec(db, cb_tutorial_load, SQL_GET_TUTORIAL, NULL);
}

void tutorial_set_user_completed(void)
{
    size_t id = user_get_current();
    sql_exec(db_tutorial.db, cb_tutorial_load,
        SQL_UPDATE_USER_TUTORIAL_COMPLETE, (int)id);
}

Tutorial* get_tutorial(size_t id)
{
    assert(id > 0 && "[TUTORIAL] Negative ID.");
    if (id > db_tutorial.tutorials.count) {
        return NULL;
    }
    return (Tutorial*)db_tutorial.tutorials.items[id];
}
