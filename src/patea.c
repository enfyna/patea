#include <assert.h>
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sql.h"
#include "text.h"

#include "func.h"
#include "lesson.h"
#include "sound.h"
#include "terminal.h"

#define TITLE "Patea"

#define WIDTH 1200
#define HEIGHT 800

#define PAGE_MAIN "main"
#define PAGE_LOGIN "login"
#define PAGE_RESULT "result_lesson"

global sqlite3* db;

global LessonState ls_state = { .lesson = NULL };
global GObject* pt_stack;

global GObject* term;

global GObject* title_term;
global GObject* title_test;
global GObject* title_main;
global GObject* title_result;

global GObject* test_question;
global GObject* test_choices[CHOICE_COUNT];

global GObject* lb_result;

global GObject* pb_test;
global GObject* pb_term;
global GObject* pb_result;

global GBytes* sound_hit;
global GBytes* sound_miss;

internal void
prepare_question(Lesson* ls)
{
    LessonQuestion question = ls->questions[ls_state.question];

    gdouble fraction = (gdouble)ls_state.correct_answers / ls->question_count;

    if (question.type == QUESTION_TERMINAL) {
        gtk_label_set_text(GTK_LABEL(title_term), ls->title);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_term), fraction);

        vte_terminal_feed(VTE_TERMINAL(term), TERM_CLEAR_SCREEN);
        vte_terminal_feed(VTE_TERMINAL(term), question.question, strlen(question.question));
        vte_terminal_feed(VTE_TERMINAL(term), TX_LESSON_CHECK_NOTICE, strlen(TX_LESSON_CHECK_NOTICE));
        vte_terminal_feed_child(VTE_TERMINAL(term), TERM_CLEAN_LINE);

        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), "question_terminal");
    } else if (question.type == QUESTION_TEST) {
        gtk_label_set_text(GTK_LABEL(title_test), ls->title);
        gtk_label_set_text(GTK_LABEL(test_question), question.question);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_test), fraction);

        for (size_t i = 0; i < CHOICE_COUNT; i++) {
            gtk_widget_set_sensitive(GTK_WIDGET(test_choices[i]), true);
            gtk_button_set_label(GTK_BUTTON(test_choices[i]), question.choice[i]);
        }

        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), "question_test");
    }
}

internal void
change_page(const char* page_name)
{
    assert(page_name != NULL);

    const gchar* current = gtk_stack_get_visible_child_name(GTK_STACK(pt_stack));

    g_print("[PAGE] from: %s => to: %s\n", current, page_name);

    if (!starts_with(page_name, LESSON_PAGE_PREFIX)) {
        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), page_name);
        return;
    }

    Lesson* lesson = lesson_get_from_name(page_name);

    assert(lesson != NULL);
    assert(ls_state.question < lesson->question_count);

    if (lesson == ls_state.lesson) {
        g_print("[LESSON] Same as before.\n");
    } else {
        g_print("[LESSON] Selected: %s\n", lesson->title);
        ls_state.correct_answers = 0;
        ls_state.question = 0;
    }

    ls_state.lesson = lesson;

    prepare_question(lesson);
}

int cb_lesson_result_get(void* data, int argc, char** argv, char** col_name)
{
    (void)data;
    (void)col_name;

    assert(argc == 4);

    int correct_count = atoi(argv[1]);
    int lesson_id = atoi(argv[3]);

    Lesson* lesson = lesson_get_from_id(lesson_id);
    assert(lesson->bt_text == NULL && "[ERROR] lesson->bt_text should always be freed beforehand.\n");
    lesson->bt_text
        = g_strdup_printf(TX_LESSON_BUTTON, lesson->title, correct_count, lesson->question_count);
    gtk_button_set_label(GTK_BUTTON(lesson->bt), lesson->bt_text);

    return 0;
}

internal void
cb_login_user(GtkWidget* widget, gpointer data)
{
    (void)widget;

    User* user = data;
    LessonDB* dbl = lesson_get_db();
    dbl->user_id = user->id;

    char buf[128] = { 0 };
    snprintf(buf, 128, TX_MAIN_TITLE, user->name);
    gtk_label_set_text(GTK_LABEL(title_main), buf);

    g_print("[LESSON] Selected user: [%d] %s\n", dbl->user_id, user->name);

    da_foreach(lesson, Lesson*, dbl->lessons)
    {
        if (lesson->bt_text != NULL) {
            free(lesson->bt_text);
        }
        lesson->bt_text = NULL;
    }

    sql_exec(cb_lesson_result_get, SQL_GET_USER_RESULTS, dbl->user_id);

    da_foreach(lesson, Lesson*, dbl->lessons)
    {
        if (lesson->bt_text == NULL) {
            lesson->bt_text = g_strdup_printf(
                TX_LESSON_BUTTON, lesson->title, 0, lesson->question_count);
            gtk_button_set_label(GTK_BUTTON(lesson->bt), lesson->bt_text);
        }
    }

    change_page(PAGE_MAIN);
}

internal void
cb_change_page(GtkWidget* widget, gpointer data)
{
    (void)widget;

    const gchar* page = data;

    change_page(page);
}

internal void
continue_test(long answer)
{
    Lesson* lesson = ls_state.lesson;

    LessonQuestion current = lesson->questions[ls_state.question];

    bool is_answer_correct = false;
    if (current.type == QUESTION_TEST) {
        g_print("[LESSON] user: %ld == answer: %d\n", answer, current.answer);
        is_answer_correct = answer == current.answer;
    } else if (current.type == QUESTION_TERMINAL) {
        for (size_t i = 0; i < CHOICE_COUNT; i++) {
            if (strlen(current.choice[i]) == 0) {
                continue;
            }
            g_print("[LESSON] Checking: %s\n", current.choice[i]);
            int res = system(current.choice[i]);
            is_answer_correct = res == 0;
            if (!is_answer_correct) {
                break;
            }
        }
    }

    if (is_answer_correct) {
        sound_play(sound_hit);
        g_print("[LESSON] Correct Answer!\n");
    } else {
        sound_play(sound_miss);
        g_print("[LESSON] Wrong Answer!\n");
    }

    ls_state.correct_answers += is_answer_correct ? 1 : -1;

    gdouble fraction = (gdouble)ls_state.correct_answers / lesson->question_count;

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_term), fraction);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_test), fraction);

    if (!is_answer_correct) {
        return;
    }

    if (++ls_state.question >= lesson->question_count) {
        gtk_label_set_text(GTK_LABEL(title_result), lesson->title);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_result), fraction);

        LessonDB* dbl = lesson_get_db();

        sql_exec(NULL, SQL_INSERT_LESSON_RESULT,
            ls_state.correct_answers, dbl->user_id, lesson->id);

        assert(lesson->bt_text != NULL && "[ERROR] lesson->bt_text should always be populated beforehand.\n");
        free(lesson->bt_text);
        lesson->bt_text = g_strdup_printf(TX_LESSON_BUTTON, lesson->title, ls_state.correct_answers, lesson->question_count);
        gtk_button_set_label(GTK_BUTTON(lesson->bt), lesson->bt_text);

        char buf[1024] = { 0 };
        sprintf(buf, TX_LESSON_RESULT,
            ls_state.correct_answers, lesson->question_count);
        gtk_label_set_text(GTK_LABEL(lb_result), buf);

        ls_state.correct_answers = 0;
        ls_state.question = 0;

        change_page(PAGE_RESULT);
        return;
    }

    prepare_question(lesson);
}

internal void
cb_send_answer(GtkWidget* widget, gpointer data)
{
    long answer = (long)data;

    gtk_widget_set_sensitive(widget, false);

    continue_test(answer);
}

internal void
cb_term_check_answer(GtkWidget* widget, gpointer data)
{
    (void)data;
    (void)widget;

    continue_test(0);
}

internal void
activate(GtkApplication* app, gpointer user_data)
{
    (void)user_data;

    GtkWidget* window;

    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    GtkBuilder* bd_main = gtk_builder_new();

    GError* error = NULL;

    if (gtk_builder_add_from_resource(bd_main, "/org/gtk/patea/ui/main.ui", &error) == 0) {
        g_error("Couldnt load file: %s, %d, %u\n", error->message, error->code, error->domain);
    }

    sound_hit = g_resources_lookup_data("/org/gtk/patea/sound/hit-sound.mp3", G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    if (sound_hit == NULL) {
        g_error("Couldnt load file: %s, %d, %u\n", error->message, error->code, error->domain);
    }

    sound_miss = g_resources_lookup_data("/org/gtk/patea/sound/miss-sound.mp3", G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    if (sound_miss == NULL) {
        g_error("Couldnt load file: %s, %d, %u\n", error->message, error->code, error->domain);
    }

    {
        title_main = gtk_builder_get_object(bd_main, "lb_main_title");
        title_term = gtk_builder_get_object(bd_main, "lb_term_title");
        title_test = gtk_builder_get_object(bd_main, "lb_test_title");
        title_result = gtk_builder_get_object(bd_main, "lb_result_title");
        test_question = gtk_builder_get_object(bd_main, "lb_test_question");
        lb_result = gtk_builder_get_object(bd_main, "lb_result");
        pb_test = gtk_builder_get_object(bd_main, "pb_test");
        pb_term = gtk_builder_get_object(bd_main, "pb_term");
        pb_result = gtk_builder_get_object(bd_main, "pb_result");

        test_choices[0] = gtk_builder_get_object(bd_main, "bt_test_a");
        g_signal_connect(test_choices[0], "clicked", G_CALLBACK(cb_send_answer), (long*)0);

        test_choices[1] = gtk_builder_get_object(bd_main, "bt_test_b");
        g_signal_connect(test_choices[1], "clicked", G_CALLBACK(cb_send_answer), (long*)1);

        test_choices[2] = gtk_builder_get_object(bd_main, "bt_test_c");
        g_signal_connect(test_choices[2], "clicked", G_CALLBACK(cb_send_answer), (long*)2);

        test_choices[3] = gtk_builder_get_object(bd_main, "bt_test_d");
        g_signal_connect(test_choices[3], "clicked", G_CALLBACK(cb_send_answer), (long*)3);
    }

    GObject* node;
    {
        node = gtk_builder_get_object(bd_main, "bt_intro_continue");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), PAGE_LOGIN);

        node = gtk_builder_get_object(bd_main, "bt_logoff");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), PAGE_LOGIN);

        node = gtk_builder_get_object(bd_main, "bt_lesson_exit0");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), PAGE_MAIN);

        node = gtk_builder_get_object(bd_main, "bt_lesson_exit1");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), PAGE_MAIN);

        node = gtk_builder_get_object(bd_main, "bt_result_return");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), PAGE_MAIN);
    }

    {
        LessonDB* dbl = lesson_get_db();

        GObject* bx_user = gtk_builder_get_object(bd_main, "bx_user");
        GObject* bx_category = gtk_builder_get_object(bd_main, "bx_category");

        da_foreach(user, User*, dbl->users)
        {
            GtkWidget* button = gtk_button_new();

            char buf[256] = { 0 };
            snprintf(buf, 256, "%s", user->name);
            gtk_button_set_label(GTK_BUTTON(button), buf);

            g_signal_connect(button, "clicked", G_CALLBACK(cb_login_user), user);

            gtk_box_pack_start(GTK_BOX(bx_user), GTK_WIDGET(button), false, true, 8);
        }

        da_for(i, dbl->categories)
        {
            GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

            GtkWidget* label = gtk_label_new(dbl->categories.items[i]);
            gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(label), false, false, 8);

            da_foreach(lesson, Lesson*, dbl->lessons)
            {
                if (lesson->category_id != (int)i) {
                    continue;
                }

                lesson->bt = gtk_button_new();

                gchar* page_name = g_strconcat(LESSON_PAGE_PREFIX, lesson->page_name, NULL);
                g_signal_connect(lesson->bt, "clicked", G_CALLBACK(cb_change_page), page_name);

                gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(lesson->bt), false, true, 8);
            }

            gtk_box_pack_start(GTK_BOX(bx_category), GTK_WIDGET(box), true, true, 8);
        }
    }

    {
        term = gtk_builder_get_object(bd_main, "term");

        node = gtk_builder_get_object(bd_main, "bt_font_size_up");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_increase_term_font), term);

        node = gtk_builder_get_object(bd_main, "bt_font_size_down");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_decrease_term_font), term);

        node = gtk_builder_get_object(bd_main, "bt_term_check_answer");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_term_check_answer), term);

        term_spawn(GTK_WIDGET(term));

        g_signal_connect(term, "commit", G_CALLBACK(cb_term_input), NULL);
        g_signal_connect(term, "eof", G_CALLBACK(cb_term_eof), NULL);
    }

    {
        pt_stack = gtk_builder_get_object(bd_main, "stack");

        gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(pt_stack));
    }

    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char** argv)
{
    int rc = sqlite3_open_v2("patea.data", &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc == SQLITE_OK) {
        g_print("[DB] Opened database successfully.\n");

        lesson_init(db);
    } else {
        g_print("[DB] Can't open database: %s\n", sqlite3_errmsg(db));
    }

    sound_init();

    GtkApplication* app;

    app = gtk_application_new("org.gtk.patea", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    rc = sqlite3_close(db);
    if (rc == SQLITE_OK) {
        g_print("[DB] Closed database successfully.\n");
    } else {
        g_error("[DB] Can't close database: %s\n", sqlite3_errmsg(db));
    }

    sound_uninit();

    return status;
}
