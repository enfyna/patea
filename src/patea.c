#include <glib.h>
#include <gio/gio.h>
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

GObject* title_term;
GObject* title_test;
GObject* title_result;

GObject* test_question;
GObject* test_choices[CHOICE_COUNT];

GObject* lb_result;

GObject* pb_test;
GObject* pb_term;
GObject* pb_result;

static void
prepare_question(Lesson* ls)
{
    LessonQuestion question = ls->questions[ls->current_question];

    gdouble fraction = (gdouble)ls->correct_answers / ls->total_questions;

    if (question.type == QUESTION_TERMINAL) {
        gtk_label_set_text(GTK_LABEL(title_test), ls->title);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_term), fraction);

        vte_terminal_feed(VTE_TERMINAL(term), CLEAR_TERMINAL_SCREEN, 6);
        vte_terminal_feed(VTE_TERMINAL(term), question.question, strlen(question.question));
        vte_terminal_feed(VTE_TERMINAL(term),
            "-------------------------------------------\r\n"
            "Soruyu cozdukten sonra kontrol et butonuna bas.\r\n",
            98);
        vte_terminal_feed_child(VTE_TERMINAL(term), CLEAN_TERMINAL_LINE, 2);

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

static void
change_page(const char* page_name)
{
    const gchar* current = gtk_stack_get_visible_child_name(GTK_STACK(pt_stack));

    g_print("[PAGE] from: %s => to: %s\n", current, page_name);

    if (!starts_with(page_name, "lesson_")) {
        gtk_stack_set_visible_child_name(GTK_STACK(pt_stack), page_name);
        return;
    }

    enum LESSONS selected = lesson_get_from_name(page_name);
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

    LessonQuestion current = lesson->questions[lesson->current_question];

    bool is_answer_correct = false;
    if (current.type == QUESTION_TEST) {
        g_print("[LESSON] Checking: user: %ld == answer: %d\n", answer, current.answer);
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
        g_print("[LESSON] Correct Answer!\n");
    } else {
        // TODO: play sound ?
        g_print("[LESSON] Wrong Answer!\n");
    }

    lesson->correct_answers += is_answer_correct ? 1 : -1;

    gdouble fraction = (gdouble)lesson->correct_answers / lesson->total_questions;

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_term), fraction);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_test), fraction);

    if (!is_answer_correct) {
        return;
    }

    if (++lesson->current_question >= lesson->total_questions) {
        gtk_label_set_text(GTK_LABEL(title_result), lesson->title);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pb_result), fraction);

        char text[1024] = { 0 };
        sprintf(text, "\n\n%d/%zu dogru cevapladin.\n\n",
            lesson->correct_answers, lesson->total_questions);
        gtk_label_set_text(GTK_LABEL(lb_result), text);

        lesson->correct_answers = 0;
        lesson->current_question = 0;

        change_page("result_lesson");
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
        g_application_quit(G_APPLICATION(app));
    }

    {
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
