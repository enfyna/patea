#define SQL_GET_USER_RESULTS "SELECT * FROM LESSON_RESULTS WHERE user_id='%d';"
#define SQL_GET_LESSONS                \
    "SELECT * FROM USERS;"             \
    "SELECT * FROM LESSON_CATEGORIES;" \
    "SELECT * FROM LESSONS;"           \
    "SELECT * FROM LESSON_RESULTS; "   \
    "SELECT * FROM LESSON_QUESTIONS;"
#define SQL_INSERT_LESSON_RESULT                        \
    "INSERT INTO "                                      \
    "LESSON_RESULTS(correct_count, user_id, lesson_id)" \
    "VALUES(%d, %d, %d)"                                \
    "ON CONFLICT(user_id, lesson_id) "                  \
    "DO UPDATE SET correct_count=excluded.correct_count;"

#define sql_exec(func, fmt, ...)                                        \
    do {                                                                \
        char buf[1024] = { 0 };                                         \
        snprintf(buf, 1024, (fmt), __VA_ARGS__);                        \
                                                                        \
        char* err;                                                      \
        int rc = sqlite3_exec(db, buf, (func), NULL, &err);             \
        if (rc == SQLITE_OK) {                                          \
            g_print("[\x1b[92mDB\x1b[0m] %s\n", buf);                   \
        } else {                                                        \
            g_error("[DB][\x1b[91mX\x1b[0m] Error(%d): %s\n", rc, err); \
        }                                                               \
    } while (0)
