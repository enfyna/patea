#include <assert.h>
#include <gtk/gtk.h>

#include "glib.h"
#include "terminal.h"
#include "utils.h"

internal void
cb_child_ready(VteTerminal* terminal, GPid pid, GError* error, gpointer user_data)
{
    (void)user_data;

    g_print("[TERMINAL] Spawn: ");
    if (error)
        g_print("Error = %d, Domain: %d, Message: %s, ", error->code, error->domain, error->message);
    g_print("PID = %d\n", pid);

    if (!terminal)
        return;

    assert(pid >= 0);
}

void term_spawn(GtkWidget* term)
{
    char* sh = "/bin/bash";
    char* arg = "-ci";
    char* cmd
        = "PS1='[patea] > ' "
          "PROMPT_COMMAND='history -c; "
          "export PROMPT_COMMAND=\"fc -ln -0 | "
          "sed '\\''s/^[ \\t]*//;s/[ \\t]*$//'\\'' >> " TERM_FIFO "\"' "
          "bash --noprofile --norc";

    gchar* command[] = {
        sh,
        arg,
        cmd,
        NULL
    };

    vte_terminal_spawn_async(VTE_TERMINAL(term),
        VTE_PTY_DEFAULT,
        "~", command,
        NULL, /* environment */
        0, /* spawn flags */
        NULL, NULL, /* child setup */
        NULL, /* child pid */
        -1, /* timeout */
        NULL, /* cancellable */
        cb_child_ready, /* callback */
        NULL); /* user_data */

    vte_terminal_set_colors(VTE_TERMINAL(term),
        &CLR_GDK(0xffffff),
        &(GdkRGBA) { .alpha = 0.85 },
        (const GdkRGBA[]) {
            CLR_GDK(0x222222),
            CLR_GDK(0xff3b47),
            CLR_GDK(0x72e900),
            CLR_GDK(0xffee00),
            CLR_GDK(0x007aff),
            CLR_GDK(0xd4007f),
            CLR_GDK(0x2eb8ac),
            CLR_GDK(0xfafafa),
            CLR_GDK(0x888888),
            CLR_GDK(0xff5a36),
            CLR_GDK(0xbaffc9),
            CLR_GDK(0xffff6d),
            CLR_GDK(0x4bb7ff),
            CLR_GDK(0xe6005c),
            CLR_GDK(0x89e5ff),
            CLR_GDK(0xffffff) },
        16);
}

void cb_increase_term_font(GtkWidget* widget, gpointer data)
{
    (void)widget;
    GtkWidget* term = data;
    gdouble scale = vte_terminal_get_font_scale(VTE_TERMINAL(term));
    scale += TERM_FONT_SCALE_FACTOR;
    vte_terminal_set_font_scale(VTE_TERMINAL(term), scale);
}

void cb_decrease_term_font(GtkWidget* widget, gpointer data)
{
    (void)widget;
    GtkWidget* term = data;
    gdouble scale = vte_terminal_get_font_scale(VTE_TERMINAL(term));
    scale -= TERM_FONT_SCALE_FACTOR;
    if (scale < TERM_MIN_FONT_SCALE)
        return;
    vte_terminal_set_font_scale(VTE_TERMINAL(term), scale);
}

void cb_term_eof(GtkWidget* term, gpointer data)
{
    (void)data;
    g_print("[TERMINAL] EOF: Spawning again...\n");

    term_spawn(term);
}
