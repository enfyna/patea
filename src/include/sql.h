#include "glib.h"

#define SQL_GET_USERS \
    "SELECT * FROM USERS;"
#define SQL_GET_USER_RESULTS \
    "SELECT * FROM LESSON_RESULTS WHERE user_id=' % d';"
#define SQL_UPDATE_USER \
    "UPDATE USERS SET %s = %s WHERE user_id = %zu;"
#define SQL_GET_TUTORIAL \
    "SELECT * FROM TUTORIAL;"
#define SQL_UPDATE_USER_TUTORIAL_COMPLETE \
    "UPDATE USERS SET tutorial = 1 WHERE user_id = %d;"
#define SQL_GET_LESSONS                \
    "SELECT * FROM LESSON_CATEGORIES;" \
    "SELECT * FROM LESSONS;"           \
    "SELECT * FROM LESSON_QUESTIONS;"
#define SQL_INSERT_LESSON_RESULT                        \
    "INSERT INTO "                                      \
    "LESSON_RESULTS(correct_count, user_id, lesson_id)" \
    "VALUES(%d, %d, %d)"                                \
    "ON CONFLICT(user_id, lesson_id) "                  \
    "DO UPDATE SET correct_count=excluded.correct_count;"

#define sql_exec(db, func, fmt, ...)                            \
    do {                                                        \
        char buf[1024] = { 0 };                                 \
        snprintf(buf, 1024, (fmt), __VA_ARGS__);                \
                                                                \
        g_print("[DB] %s ", buf);                               \
        char* err;                                              \
        int rc = sqlite3_exec(db, buf, (func), NULL, &err);     \
        if (rc == SQLITE_OK) {                                  \
            g_print("\x1b[92mOK\x1b[0m\n");                     \
        } else {                                                \
            g_error("\x1b[91mERROR\x1b[0m(%d): %s\n", rc, err); \
        }                                                       \
    } while (0)
