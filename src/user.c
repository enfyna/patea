#include <assert.h>

#include "sql.h"
#include "user.h"

UserDB db_user = { 0 };

int cb_user_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)argc;

    int used_arg_count = 0;

    if (0 == strcmp(col_name[0], "user_id")) {
        int user_id = atoi(argv[used_arg_count++]);
        bool user_tutorial = atoi(argv[used_arg_count++]);
        char* user_name = strdup(argv[used_arg_count++]);
        assert(used_arg_count == argc && "SQL Table Changed!");

        User* new = malloc(sizeof(User));
        new->id = user_id;
        new->name = user_name;
        new->tutorial = user_tutorial;

        da_push_to_id(db_user.users, User*, new, user_id);
    } else {
        assert(false && "SQL Table Changed!");
    }

    return 0;
}

void user_init(sqlite3* db)
{
    da_init(User*, db_user.users);

    db_user.db = db;

    sql_exec(db_user.db, cb_user_load, SQL_GET_USERS, NULL);
}

void user_update(const char* key, const char* value)
{
    sql_exec(db_user.db, NULL, SQL_UPDATE_USER, key, value, db_user.id_user);
    user_init(db_user.db);
}

da user_get_users(void)
{
    return db_user.users;
}

void user_set_current(size_t id)
{
    db_user.id_user = id;
}

size_t user_get_current(void)
{
    return db_user.id_user;
}
