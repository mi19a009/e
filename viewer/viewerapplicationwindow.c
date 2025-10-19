/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_ABOUT "show-about"
#define PROPERTY_APPLICATION               "application"
#define PROPERTY_SHOW_MENUBAR              "show-menubar"
#define SIGNAL_DESTROY "destroy"
#define RESOURCE_ABOUT        "gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define TITLE _("Picture Viewer")

/* Viewer Application Window クラスのインスタンス */
struct _ViewerApplicationWindow
{
	GtkApplicationWindow parent_instance;
};

static void viewer_application_window_activate_about    (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_application_window_class_init        (ViewerApplicationWindowClass *self);
static void viewer_application_window_init              (ViewerApplicationWindow *self);

/* Viewer Application Window クラス */
G_DEFINE_TYPE (ViewerApplicationWindow, viewer_application_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー項目アクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ ACTION_ABOUT, viewer_application_window_activate_about, NULL, NULL, NULL },
};

/*******************************************************************************
バージョン情報ダイアログを表示します。
*/
static void
viewer_application_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *builder;
	GtkWindow *dialog;
	char path [VIEWER_RESOURCE_CCH];
	viewer_get_resource_path (path, VIEWER_RESOURCE_CCH, RESOURCE_ABOUT);
	builder = gtk_builder_new_from_resource (path);
	dialog = GTK_WINDOW (gtk_builder_get_object (builder, RESOURCE_ABOUT_DIALOG));
	g_signal_connect_swapped (dialog, SIGNAL_DESTROY, G_CALLBACK (gtk_window_destroy), dialog);
	gtk_window_set_destroy_with_parent (dialog, TRUE);
	gtk_window_set_modal (dialog, TRUE);
	gtk_window_set_transient_for (dialog, GTK_WINDOW (user_data));
	gtk_window_present (dialog);
	g_object_unref (builder);
}

/*******************************************************************************
クラスを初期化します。
*/
static void
viewer_application_window_class_init (ViewerApplicationWindowClass *self)
{
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_window_init (ViewerApplicationWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/
GtkWidget *
viewer_application_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_APPLICATION_WINDOW,
		PROPERTY_APPLICATION, application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}
