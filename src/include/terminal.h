#pragma once

#include <gtk/gtk.h>
#include <vte-2.91/vte/vte.h>

#define CLEAR_TERMINAL_SCREEN "\033[2J\r\n", 6
#define CLEAN_TERMINAL_LINE "\x15\n", 2
#define CHECK_BUTTON_NOTICE                                \
    "-----------------------------------------------\r\n"  \
    "Soruyu cozdukten sonra kontrol et butonuna bas.\r\n", \
        102

#define TERM_FONT_SCALE_FACTOR (0.2)

#define CLR_R(x) (((x) & 0xff0000) >> 16)
#define CLR_G(x) (((x) & 0x00ff00) >> 8)
#define CLR_B(x) (((x) & 0x0000ff) >> 0)
#define CLR_16(x) ((double)(x) / 0xff)
#define CLR_GDK(x) (const GdkRGBA) { .red = CLR_16(CLR_R(x)), \
    .green = CLR_16(CLR_G(x)),                                \
    .blue = CLR_16(CLR_B(x)),                                 \
    .alpha = 0 }

void term_spawn(GtkWidget* term);
void cb_increase_term_font(GtkWidget* widget, gpointer data);
void cb_decrease_term_font(GtkWidget* widget, gpointer data);
void cb_term_input(GtkWidget* widget, gpointer data);
void cb_term_eof(GtkWidget* widget, gpointer data);
void child_ready(VteTerminal* terminal, GPid pid, GError* error, gpointer user_data);
