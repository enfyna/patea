#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sql.h"
#include "user.h"

UserDB db_user = { 0 };

int cb_user_load(void* data, int argc, char** argv, char** col_name)
{
    (void)data;

    User* new = malloc(sizeof(User));

    size_t used_arg_count = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(col_name[i], "name")) {
            new->name = strdup(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "user_id")) {
            new->id = atoi(argv[i]);
            used_arg_count++;
        } else if (!strcmp(col_name[i], "tutorial")) {
            new->tutorial = atoi(argv[i]);
            used_arg_count++;
        }
    }

    assert(used_arg_count == 3 && "User Table Changed!");

    da_push(db_user.users, User*, new);

    return 0;
}

void user_init(sqlite3* db)
{
    da_init(User*, db_user.users);

    db_user.db = db;

    sql_exec(db_user.db, cb_user_load, NULL, SQL_GET_USERS);
}

void user_update(const char* key, const char* value)
{
    sql_exec(db_user.db, NULL, NULL, SQL_UPDATE_USER, key, value, db_user.id_user);

    da_free(db_user.users);

    user_init(db_user.db);
}

da user_get_users(void)
{
    return db_user.users;
}

User* user_get_user(size_t id)
{
    for (size_t i = 0; i < db_user.users.count; i++) {
        User* user = db_user.users.items[i];
        if (user->id == (int)id) {
            return user;
        }
    }
    return NULL;
}

void user_set_current(size_t id)
{
    db_user.id_user = id;
}

size_t user_get_current(void)
{
    return db_user.id_user;
}
