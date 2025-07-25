#include <assert.h>
#include <fcntl.h>
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gdk-pixbuf/gdk-pixbuf.h"
#include "sql.h"

#include "lesson.h"
#include "sound.h"
#include "terminal.h"
#include "text.h"
#include "tutorial.h"
#include "user.h"
#include "utils.h"

#define TITLE "Patea"

#define WIDTH 1200
#define HEIGHT 800

#define PAGE_MAIN "main"
#define PAGE_LOGIN "login"
#define PAGE_TUTORIAL "tutorial"
#define PAGE_RESULT "result_lesson"

#define BUF_SIZE 256

global LessonState ls_state = { .ls = NULL, .qs = NULL };
global size_t tutorial_pos = 0;
global GObject* pt_stack;

global GObject* term;

global GObject* title_term;
global GObject* title_test;
global GObject* title_main;
global GObject* title_result;

global GObject* test_question;
global GObject* test_choices[LS_CHOICE_COUNT];

global GObject* lb_result;
global GObject* lb_tutorial;

global GObject* im_tutorial;
global GObject* im_lesson_test;
global GObject* im_lesson_term;

global GObject* pb_test;
global GObject* pb_term;
global GObject* pb_result;

global GBytes* sound_hit;
global GBytes* sound_miss;

global int t_fifo = 0;

internal void
cb_tutorial_continue(GtkWidget* widget, gpointer data);

internal void
prepare_question(void)
{
    if (ls_state.qs != NULL) {
        free(ls_state.qs);
    }
    ls_state.qs = lesson_get_question(ls_state.ls->id, ls_state.q_pos);

    gboolean is_term = ls_state.qs->type == QUESTION_TERMINAL;

    gtk_label_set_text(
        GTK_LABEL(is_term ? title_term : title_test), ls_state.ls->title);

    gfloat fraction = ls_state.correct_answers / (gfloat)ls_state.ls->question_count;
    gtk_progress_bar_set_fraction(
        GTK_PROGRESS_BAR(is_term ? pb_term : pb_test), fraction);

    GObject* image = is_term ? im_lesson_term : im_lesson_test;
    if (strlen(ls_state.qs->image) > 0) {
        GdkPixbuf* gp = gdk_pixbuf_new_from_resource_at_scale(ls_state.qs->image, 200, 200, FALSE, NULL);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), gp);
    } else {
        gtk_image_clear(GTK_IMAGE(image));
    }

    if (is_term) {
        vte_terminal_feed(VTE_TERMINAL(term), TERM_CLEAR_SCREEN);
        vte_terminal_feed(VTE_TERMINAL(term), ls_state.qs->question, strlen(ls_state.qs->question));
        vte_terminal_feed(VTE_TERMINAL(term), TX_LESSON_CHECK_NOTICE, strlen(TX_LESSON_CHECK_NOTICE));
        vte_terminal_feed_child(VTE_TERMINAL(term), TERM_CLEAN_LINE);
    } else if (ls_state.qs->type == QUESTION_TEST) {
        gtk_label_set_text(GTK_LABEL(test_question), ls_state.qs->question);
        for (size_t i = 0; i < LS_CHOICE_COUNT; i++) {
            gtk_widget_set_sensitive(GTK_WIDGET(test_choices[i]), true);
            gtk_button_set_label(GTK_BUTTON(test_choices[i]), ls_state.qs->choice[i]);
        }
    }

    gtk_stack_set_visible_child_name(
        GTK_STACK(pt_stack), is_term ? "question_terminal" : "question_test");
}

internal void
change_page(const char* page_name)
{
    assert(page_name != NULL);

    const gchar* current = gtk_stack_get_visible_child_name(GTK_STACK(pt_stack));

    g_print("[PAGE] from: %s => to: %s\n", current, page_name);

    if (starts_with(page_name, PAGE_TUTORIAL)) {
        cb_tutorial_continue(NULL, (long*)1);
        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), page_name);
        return;
    }

    if (!starts_with(page_name, LS_PREFIX)) {
        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), page_name);
        return;
    }

    Lesson* lesson = lesson_get_from_name(page_name);

    assert(lesson != NULL);
    assert(ls_state.q_pos < lesson->question_count);

    if (lesson == ls_state.ls) {
        g_print("[LESSON] Same as before.\n");
    } else {
        g_print("[LESSON] Selected: %s\n", lesson->title);
        ls_state.correct_answers = 0;
        ls_state.q_pos = 0;
        ls_state.qs = NULL;
    }

    ls_state.ls = lesson;

    prepare_question();
}

internal void
cb_tutorial_continue(GtkWidget* widget, gpointer data)
{
    (void)widget;

    long step = (long)data;
    tutorial_pos += step;
    if (tutorial_pos < 1) {
        change_page(PAGE_LOGIN);
        return;
    }

    Tutorial* t = get_tutorial(tutorial_pos);
    if (t == NULL) {
        user_update("tutorial", "1");
        change_page(PAGE_MAIN);
    } else {
        gtk_label_set_text(GTK_LABEL(lb_tutorial), t->text);
        if (strlen(t->image) > 0) {
            GdkPixbuf* gp = gdk_pixbuf_new_from_resource_at_scale(t->image, 200, 200, FALSE, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(im_tutorial), gp);
        } else {
            gtk_image_set_from_icon_name(
                GTK_IMAGE(im_tutorial), "gtk-info", GTK_ICON_SIZE_DIALOG);
        }
    }
    free(t);
}

internal void
cb_login_user(GtkWidget* widget, gpointer data)
{
    (void)widget;

    long user_id = (long)data;
    User* user = user_get_user(user_id);

    user_set_current(user->id);
    tutorial_pos = 0;

    char buf[128] = { 0 };
    snprintf(buf, sizeof buf, TX_MAIN_TITLE, user->name);
    gtk_label_set_text(GTK_LABEL(title_main), buf);

    g_print("[LESSON] Selected user: [%d] %s\n", user->id, user->name);

    lesson_update_bt_texts(user->id);

    user->tutorial == false ? change_page(PAGE_TUTORIAL)
                            : change_page(PAGE_MAIN);
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
    Lesson* lesson = ls_state.ls;

    bool is_answer_correct = false;
    if (ls_state.qs->type == QUESTION_TEST) {
        g_print("[LESSON] user: %ld == answer: %d\n", answer, ls_state.qs->answer);
        is_answer_correct = answer == ls_state.qs->answer;
    } else if (ls_state.qs->type == QUESTION_TERMINAL) {
        if (ls_state.qs->answer == 2) {
            assert(t_fifo >= 0 && "fifo not open!");

            char buf[BUF_SIZE] = { 0 };
            int buf_read = 0;

            do {
                buf_read = read(t_fifo, buf, BUF_SIZE);
                assert(buf_read >= 0 && "fifo destroyed ?");

                g_print("[FIFO] Buf Read: %d\n", buf_read);
                if (buf_read == 0) {
                    return;
                }

                for (char* cut = strtok(buf, "\n"); cut != NULL; cut = strtok(NULL, "\n")) {
                    g_print("[LESSON] Checking: |%s| == |%s|\n", cut, ls_state.qs->choice[0]);
                    if (!strcmp(cut, ls_state.qs->choice[0])) {
                        is_answer_correct = true;
                        break;
                    }
                }
            } while (buf_read >= BUF_SIZE);

        } else if (ls_state.qs->answer == 1) {
            for (size_t i = 0; i < LS_CHOICE_COUNT; i++) {
                if (strlen(ls_state.qs->choice[i]) == 0) {
                    continue;
                }
                g_print("[LESSON] Checking: %s\n", ls_state.qs->choice[i]);
                int res = system(ls_state.qs->choice[i]);

                is_answer_correct = res == 0;
                if (!is_answer_correct) {
                    break;
                }
            }
        } else {
            assert(false && "Invalid terminal question type!");
        }
    }

    if (is_answer_correct) {
        g_print("[LESSON] Correct Answer!\n");
        sound_play(sound_hit);
    } else {
        g_print("[LESSON] Wrong Answer!\n");
        sound_play(sound_miss);
    }

    ls_state.correct_answers += is_answer_correct ? 1 : -1;

    gdouble fraction = (gdouble)ls_state.correct_answers / lesson->question_count;

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_term), fraction);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_test), fraction);

    if (!is_answer_correct) {
        return;
    }

    free(ls_state.qs);
    ls_state.qs = NULL;

    if (++ls_state.q_pos >= lesson->question_count) {
        gtk_label_set_text(GTK_LABEL(title_result), lesson->title);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_result), fraction);

        lesson_set_user_result(
            user_get_current(),
            lesson->id,
            ls_state.correct_answers);

        char buf[BUF_SIZE] = { 0 };
        snprintf(buf, sizeof buf, TX_LESSON_BUTTON,
            lesson->title, ls_state.correct_answers / (float)lesson->question_count * 100);
        gtk_button_set_label(GTK_BUTTON(lesson->bt), buf);

        snprintf(buf, sizeof buf, TX_LESSON_RESULT,
            ls_state.correct_answers, lesson->question_count);
        gtk_label_set_text(GTK_LABEL(lb_result), buf);

        ls_state.correct_answers = 0;
        ls_state.q_pos = 0;
        ls_state.qs = 0;

        change_page(PAGE_RESULT);
        return;
    }

    prepare_question();
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

    GError* err = NULL;

    if (gtk_builder_add_from_resource(bd_main, "/org/gtk/patea/ui/main.ui", &err) == 0) {
        g_error("Couldnt load file: %s, %d, %u\n", err->message, err->code, err->domain);
    }

    sound_hit = g_resources_lookup_data("/org/gtk/patea/sound/hit", G_RESOURCE_LOOKUP_FLAGS_NONE, &err);
    if (sound_hit == NULL) {
        g_error("Couldnt load file: %s, %d, %u\n", err->message, err->code, err->domain);
    }

    sound_miss = g_resources_lookup_data("/org/gtk/patea/sound/miss", G_RESOURCE_LOOKUP_FLAGS_NONE, &err);
    if (sound_miss == NULL) {
        g_error("Couldnt load file: %s, %d, %u\n", err->message, err->code, err->domain);
    }

    {
        title_main = gtk_builder_get_object(bd_main, "lb_main_title");
        title_term = gtk_builder_get_object(bd_main, "lb_term_title");
        title_test = gtk_builder_get_object(bd_main, "lb_test_title");
        title_result = gtk_builder_get_object(bd_main, "lb_result_title");
        test_question = gtk_builder_get_object(bd_main, "lb_test_question");
        lb_result = gtk_builder_get_object(bd_main, "lb_result");
        lb_tutorial = gtk_builder_get_object(bd_main, "lb_tutorial");
        im_tutorial = gtk_builder_get_object(bd_main, "im_tutorial");
        im_lesson_test = gtk_builder_get_object(bd_main, "im_lesson_test");
        im_lesson_term = gtk_builder_get_object(bd_main, "im_lesson_term");
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

        node = gtk_builder_get_object(bd_main, "bt_tutorial_continue");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_tutorial_continue), (long*)1);

        node = gtk_builder_get_object(bd_main, "bt_tutorial_return");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_tutorial_continue), (long*)-1);

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
        da users = user_get_users();

        GObject* bx_user = gtk_builder_get_object(bd_main, "bx_user");
        GObject* bx_category = gtk_builder_get_object(bd_main, "bx_category");

        for (size_t i = 0; i < users.count; i++) {
            User* user = users.items[i];
            GtkWidget* button = gtk_button_new();

            gtk_button_set_label(GTK_BUTTON(button), user->name);

            g_signal_connect(button, "clicked", G_CALLBACK(cb_login_user), (long*)(long)user->id);

            gtk_box_pack_start(GTK_BOX(bx_user), GTK_WIDGET(button), false, true, 8);
        }

        da lessons = lesson_get_lessons();
        da categories = lesson_get_categories();
        for (size_t i = 0; i < categories.count; i++) {
            LessonCategory* cat = categories.items[i];
            GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

            GtkWidget* label = gtk_label_new(cat->name);
            gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(label), false, false, 8);

            for (size_t i = 0; i < lessons.count; i++) {
                Lesson* lesson = lessons.items[i];

                if (lesson->category_id != (int)cat->id) {
                    continue;
                }

                lesson->bt = gtk_button_new();

                gchar* page_name = g_strconcat(LS_PREFIX, lesson->page_name, NULL);
                g_signal_connect(lesson->bt, "clicked", G_CALLBACK(cb_change_page), page_name);

                gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(lesson->bt), false, true, 8);
            }

            gtk_box_pack_start(GTK_BOX(bx_category), GTK_WIDGET(box), true, true, 8);
        }
        da_free(categories);
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
    int res;
    do {
        res = mkfifo(TERM_FIFO, 0600);
        if (res == 0) {
            g_print("[FIFO] Created fifo successfully.\n");
        } else if (res == -1 && errno == 17) {
            g_print("[FIFO] Existing fifo found. Removing and starting fresh.\n");
            res = remove(TERM_FIFO);
            continue;
        } else {
            g_error("[FIFO] Error(%d): %s\n", errno, strerror(errno));
        }
        break;
    } while (1);

    t_fifo = open(TERM_FIFO, O_RDONLY | O_NONBLOCK);
    if (t_fifo > 0) {
        g_print("[FIFO] Opened fifo successfully.\n");
    } else {
        g_error("[FIFO] Error(%d): %s\n", errno, strerror(errno));
    }

    sqlite3* db = NULL;

    int rc = sqlite3_open_v2("patea.data", &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc == SQLITE_OK) {
        g_print("[DB] Opened database successfully.\n");

        user_init(db);
        lesson_init(db);
        tutorial_init(db);
    } else {
        g_print("[DB] Can't open database: %s\n", sqlite3_errmsg(db));
    }

    sound_init();

    GtkApplication* app;

    app = gtk_application_new("org.gtk.patea", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    close(t_fifo);
    res = remove(TERM_FIFO);
    if (res == 0) {
        g_print("[FIFO] Removed fifo successfully.\n");
    } else {
        g_print("[FIFO] Error(%d): %s\n", errno, strerror(errno));
    }

    rc = sqlite3_close(db);
    if (rc == SQLITE_OK) {
        g_print("[DB] Closed database successfully.\n");
    } else {
        g_error("[DB] Error(%d): %s\n", rc, sqlite3_errmsg(db));
    }

    sound_uninit();

    return status;
}
