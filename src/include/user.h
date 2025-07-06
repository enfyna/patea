#pragma once

#include <sqlite3.h>
#include <stdbool.h>
#include <stddef.h>

#include "utils.h"

typedef struct {
    int id;
    bool tutorial;
    char* name;
} User;

typedef struct {
    size_t id_user;
    da users;
    sqlite3* db;
} UserDB;

void user_init(sqlite3* db);
da user_get_users(void);
void user_set_current(size_t id);
size_t user_get_current(void);
void user_update(const char* key, const char* value);
