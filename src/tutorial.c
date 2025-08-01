#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "sql.h"
#include "tutorial.h"

TutorialDB db_tutorial = { 0 };

int cb_tutorial_load(void* data, int argc, char** argv, char** col_name)
{
    Tutorial** lt = data;
    Tutorial* p = calloc(1, sizeof(Tutorial));
    *lt = p;

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "id")) {
            p->id = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "text")) {
            strncpy(p->text, argv[i], sizeof p->text);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "image")) {
            strncpy(p->image, argv[i], sizeof p->image);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 3 && "Tutorial table changed");

    return 0;
}

Tutorial* get_tutorial_alloc(size_t id)
{
    Tutorial* t = NULL;

    sql_exec(db_tutorial.db, cb_tutorial_load, &t, SQL_GET_TUTORIAL, id);

    return t;
}

void tutorial_init(sqlite3* db)
{
    db_tutorial.db = db;
}
