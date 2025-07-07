#include "glib.h"

#define SQL_GET_USERS \
    "SELECT * FROM USERS;"
#define SQL_GET_USERS_ARGS ""

#define SQL_GET_USER_LIST \
    "SELECT name FROM USERS;"
#define SQL_GET_USER_LIST_ARGS ""

#define SQL_GET_USER_RESULTS \
    "SELECT * FROM LESSON_RESULTS WHERE user_id= %d;"
#define SQL_GET_USER_RESULTS_ARGS "(%d)"

#define SQL_UPDATE_USER \
    "UPDATE USERS SET %s = %s WHERE user_id = %zu;"
#define SQL_UPDATE_USER_ARGS "(%s,%s,%zu)"

#define SQL_GET_TUTORIAL \
    "SELECT * FROM TUTORIAL WHERE id = %zu;"
#define SQL_GET_TUTORIAL_ARGS "(%zu)"

#define SQL_UPDATE_USER_TUTORIAL_COMPLETE \
    "UPDATE USERS SET tutorial = 1 WHERE user_id = %d;"
#define SQL_UPDATE_USER_TUTORIAL_COMPLETE_ARGS "(%d)"

#define SQL_GET_LESSONS \
    "SELECT * FROM LESSONS;"
#define SQL_GET_LESSONS_ARGS ""

#define SQL_GET_LESSON_QUESTION \
    "SELECT * FROM LESSON_QUESTIONS WHERE lesson_id = %zu AND question_pos = %zu;"
#define SQL_GET_LESSON_QUESTION_ARGS "(%zu,%zu)"

#define SQL_GET_LESSON_CATEGORIES \
    "SELECT * FROM LESSON_CATEGORIES;"
#define SQL_GET_LESSON_CATEGORIES_ARGS ""

#define SQL_INSERT_LESSON_RESULT                        \
    "INSERT INTO "                                      \
    "LESSON_RESULTS(correct_count, user_id, lesson_id)" \
    "VALUES(%d, %d, %d)"                                \
    "ON CONFLICT(user_id, lesson_id) "                  \
    "DO UPDATE SET correct_count=excluded.correct_count;"
#define SQL_INSERT_LESSON_RESULT_ARGS "(%d, %d, %d)"

#define sql_exec(db, func, data, fmt, ...)                      \
    do {                                                        \
        char buf[256] = { 0 };                                  \
        snprintf(buf, 256, (fmt), __VA_ARGS__);                 \
                                                                \
        g_print("[DB] %s" fmt##_ARGS " ", #fmt, __VA_ARGS__);   \
        char* err;                                              \
        int rc = sqlite3_exec(db, buf, (func), (data), &err);   \
        if (rc == SQLITE_OK) {                                  \
            g_print("\x1b[92mOK\x1b[0m\n");                     \
        } else {                                                \
            g_error("\x1b[91mERROR\x1b[0m(%d): %s\n", rc, err); \
        }                                                       \
    } while (0)
