#define PURPLE_PLUGINS

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"

#include "gtkconv.h"
#include "gtkplugin.h"

PurplePlugin *pastesend_plugin = NULL;

static void paste_and_send(GtkWidget *menu_entry, PidginConversation *gtkconv)
{
    g_return_if_fail(gtkconv != NULL);

    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_paste_clipboard(gtkconv->entry_buffer, clipboard, NULL, TRUE);
    g_signal_emit_by_name(gtkconv->entry, "message_send");
}

static void right_click_popup(GtkTextView *text_view, GtkMenu *menu, PidginConversation *gtkconv)
{
    GtkWidget *menu_entry = gtk_menu_item_new_with_label("Paste & Send");
    gtk_widget_set_tooltip_text(menu_entry, "Pastes the current clipboard text and presses \"send\".");
    gtk_menu_insert(GTK_MENU(menu), menu_entry, 1);

    g_signal_connect(G_OBJECT(menu_entry), "activate", G_CALLBACK(paste_and_send), gtkconv);
    gtk_widget_show(menu_entry);
}

static void attach_to_conversation(PurpleConversation *conv)
{
    PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
    GtkTextView *view = GTK_TEXT_VIEW(gtkconv->entry);

    g_signal_connect(G_OBJECT(view), "populate-popup", G_CALLBACK(right_click_popup), gtkconv);
}

static void detach_from_conversation(PurpleConversation *conv)
{
    PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
    GtkTextView *view = GTK_TEXT_VIEW(gtkconv->entry);

    g_signal_handlers_disconnect_matched(G_OBJECT(view), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, G_CALLBACK(right_click_popup), NULL);
}

static gboolean plugin_load(PurplePlugin *plugin)
{
    pastesend_plugin = plugin;
    GList *convs;
    for (convs = purple_get_conversations(); convs != NULL; convs = convs->next) {
        attach_to_conversation((PurpleConversation *)convs->data);
    }

    void *conv_handle = purple_conversations_get_handle();
    purple_signal_connect(conv_handle, "conversation-created", plugin, PURPLE_CALLBACK(attach_to_conversation), NULL);

    return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin)
{
    GList *convs;
    for (convs = purple_get_conversations(); convs != NULL; convs = convs->next) {
        PurpleConversation *conv = (PurpleConversation *)convs->data;
        if(PIDGIN_IS_PIDGIN_CONVERSATION(conv)) {
            detach_from_conversation(conv);
        }
    }
    return TRUE;
}

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-Paste_Send",
    "Paste & Send",
    "1.0",

    "Paste & Send Plugin",
    "Paste & Send Plugin",
    "4lab group",
    "https://github.com/4lab",

    plugin_load,
    plugin_unload,
    NULL,

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static void init_plugin(PurplePlugin *plugin)
{ }

PURPLE_INIT_PLUGIN(Paste_Send, init_plugin, info)
