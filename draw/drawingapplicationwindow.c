/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "drawing.h"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define RESOURCE_ABOUT        "gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define SIGNAL_DESTROY        "destroy"

/* Drawing Application Window クラスのインスタンス */
struct _DrawingApplicationWindow
{
	GtkApplicationWindow parent_instance;
};

static void drawing_application_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void drawing_application_window_class_init     (DrawingApplicationWindowClass *this_class);
static void drawing_application_window_init           (DrawingApplicationWindow *self);

/* Drawing Application Window クラス */
G_DEFINE_TYPE (DrawingApplicationWindow, drawing_application_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー項目アクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ "show-about", drawing_application_window_activate_about, NULL, NULL, NULL },
};

/*******************************************************************************
バージョン情報ダイアログを表示します。
*/
static void
drawing_application_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *builder;
	GtkWindow *dialog;
	char path [DRAWING_RESOURCE_PATH_CCH];
	drawing_get_resource_path (path, DRAWING_RESOURCE_PATH_CCH, RESOURCE_ABOUT);
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
drawing_application_window_class_init (DrawingApplicationWindowClass *this_class)
{
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
drawing_application_window_init (DrawingApplicationWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/
GtkWidget *
drawing_application_window_new (GApplication *application)
{
	return g_object_new (DRAWING_TYPE_APPLICATION_WINDOW,
		PROPERTY_APPLICATION,  application,
		PROPERTY_SHOW_MENUBAR, TRUE,
		NULL);
}
