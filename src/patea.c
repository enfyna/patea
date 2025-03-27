#include <glib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "func.h"
#include "lesson.h"
#include "terminal.h"

#define WIDTH 1200
#define HEIGHT 800

LessonState ls_state = { .current = LESSON_NOT_SELECTED };
GObject* pt_stack;

GObject* term;
GObject* term_title;
GObject* test_title;
GObject* test_question;
GObject* test_a;
GObject* test_b;
GObject* test_c;
GObject* test_d;
GObject* lb_result;

static void
prepare_question(Lesson* ls)
{
    if (ls->questions[ls->current_question].type == QUESTION_TERMINAL) {
        gtk_label_set_text(GTK_LABEL(test_title), ls->title);

        vte_terminal_feed(VTE_TERMINAL(term), CLEAR_TERMINAL_SCREEN, 6);
        vte_terminal_feed(VTE_TERMINAL(term),
            ls->questions[ls->current_question].question,
            strlen(ls->questions[ls->current_question].question));
        vte_terminal_feed_child(VTE_TERMINAL(term), CLEAN_TERMINAL_LINE, 2);

        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), "question_terminal");
    } else if (ls->questions[ls->current_question].type == QUESTION_TEST) {
        gtk_label_set_text(GTK_LABEL(test_title), ls->title);
        gtk_label_set_text(GTK_LABEL(test_question), ls->questions[ls->current_question].question);

        gtk_widget_set_sensitive(GTK_WIDGET(test_a), true);
        gtk_widget_set_sensitive(GTK_WIDGET(test_b), true);
        gtk_widget_set_sensitive(GTK_WIDGET(test_c), true);
        gtk_widget_set_sensitive(GTK_WIDGET(test_d), true);

        gtk_button_set_label(GTK_BUTTON(test_a), ls->questions[ls->current_question].a);
        gtk_button_set_label(GTK_BUTTON(test_b), ls->questions[ls->current_question].b);
        gtk_button_set_label(GTK_BUTTON(test_c), ls->questions[ls->current_question].c);
        gtk_button_set_label(GTK_BUTTON(test_d), ls->questions[ls->current_question].d);

        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), "question_test");
    }
}

static void
change_page(const char* page)
{
    const gchar* current = gtk_stack_get_visible_child_name(GTK_STACK(pt_stack));

    g_print("[PAGE] from: %s => to: %s\n", current, page);

    if (!starts_with(page, "lesson_")) {
        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), page);
        return;
    }

    enum LESSONS selected = lesson_get_from_page(page);
    if (selected == LESSON_NOT_SELECTED) {
        return; // should not happen
    }

    Lesson* lesson = lesson_get_from_enum(selected);
    if (lesson->current_question >= lesson->total_questions) {
        return;
    }

    if (selected == ls_state.current) {
        g_print("[LESSON] Same as before.\n");
    } else {
        g_print("[LESSON] Selected: %s\n", lesson->title);
    }

    ls_state.current = selected;

    prepare_question(lesson);
}

static void
cb_change_page(GtkWidget* widget, gpointer data)
{
    (void)widget;

    const gchar* page = data;

    change_page(page);
}

static void
continue_test(long answer)
{
    Lesson* lesson = lesson_get_from_enum(ls_state.current);

    if (lesson->questions[lesson->current_question].type == QUESTION_TEST) {
        if (answer == lesson->questions[lesson->current_question].answer) {
            lesson->correct_answers++;
        } else {
            // TODO: play sound ?
            lesson->correct_answers--;
            return;
        }
    } else if (lesson->questions[lesson->current_question].type == QUESTION_TERMINAL) {
        g_print("[LESSON] Checking: %s\n", lesson->questions[lesson->current_question].a);
        int res = system(lesson->questions[lesson->current_question].a);
        if (res != 0) {
            lesson->correct_answers--;
            g_print("[LESSON] Fail!\n");
            return;
        }
        lesson->correct_answers++;
        g_print("[LESSON] Success!\n");
    }

    if (++lesson->current_question >= lesson->total_questions) {
        char* text = calloc(1, 1024);
        sprintf(text, "%s\n\n%d/%d dogru cevapladin.\n\n", 
            lesson->title, lesson->correct_answers, lesson->total_questions);

        gtk_label_set_text(GTK_LABEL(lb_result), text);
        free(text);

        lesson->correct_answers = 0;
        lesson->current_question = 0;

        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), "lesson_result");
        return;
    }

    prepare_question(lesson);
}

static void
cb_send_answer(GtkWidget* widget, gpointer data)
{
    long answer = (long)data;

    gtk_widget_set_sensitive(widget, false);

    continue_test(answer);
}

static void
cb_term_check_answer(GtkWidget* widget, gpointer data)
{
    (void)data;
    (void)widget;

    continue_test(0);
}

static void
activate(GtkApplication* app, gpointer user_data)
{
    (void)user_data;
    GtkWidget* window;

    window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "Patea");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);

    GtkBuilder* bd_main;

    GObject* node;

    GError* error = NULL;

    bd_main = gtk_builder_new();
    if (gtk_builder_add_from_resource(bd_main, "/org/gtk/patea/ui/main.ui", &error) == 0) {
        g_printerr("[ERROR] Couldnt load file: %s\n", error->message);
        g_clear_error(&error);
        gtk_main_quit();
    }

    {
        term_title = gtk_builder_get_object(bd_main, "lb_term_title");
        test_title = gtk_builder_get_object(bd_main, "lb_test_title");
        test_question = gtk_builder_get_object(bd_main, "lb_test_question");
        lb_result = gtk_builder_get_object(bd_main, "lb_lesson_result");

        test_a = gtk_builder_get_object(bd_main, "bt_test_a");
        g_signal_connect(test_a, "clicked", G_CALLBACK(cb_send_answer), (long*)0);

        test_b = gtk_builder_get_object(bd_main, "bt_test_b");
        g_signal_connect(test_b, "clicked", G_CALLBACK(cb_send_answer), (long*)1);

        test_c = gtk_builder_get_object(bd_main, "bt_test_c");
        g_signal_connect(test_c, "clicked", G_CALLBACK(cb_send_answer), (long*)2);

        test_d = gtk_builder_get_object(bd_main, "bt_test_d");
        g_signal_connect(test_d, "clicked", G_CALLBACK(cb_send_answer), (long*)3);

        node = gtk_builder_get_object(bd_main, "bt_intro_continue");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "main");

        node = gtk_builder_get_object(bd_main, "bt_lesson_exit0");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "main");

        node = gtk_builder_get_object(bd_main, "bt_lesson_exit1");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "main");

        node = gtk_builder_get_object(bd_main, "bt_result_return");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "main");

        node = gtk_builder_get_object(bd_main, "bt_main_select_l1");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "lesson_terminal_1");

        node = gtk_builder_get_object(bd_main, "bt_main_select_l2");
        g_signal_connect(node, "clicked", G_CALLBACK(cb_change_page), "lesson_test_1");
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

        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    }

    {
        pt_stack = gtk_builder_get_object(bd_main, "stack");

        gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(pt_stack));
    }

    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char** argv)
{
    lesson_init();

    GtkApplication* app;

    app = gtk_application_new("org.gtk.patea", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}
