/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "drawing.h"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"

/* Drawing Application クラスのインスタンス */
struct _DrawingApplication
{
	GtkApplication parent_instance;
};

/* Drawing Application クラスのアクセラレーター */
struct _DrawingApplicationAccelEntry
{
	const char  *detailed_action_name;
	const char **accels;
};

typedef struct _DrawingApplicationAccelEntry DrawingApplicationAccelEntry;
static void drawing_application_activate               (GApplication *self);
static void drawing_application_activate_new           (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void drawing_application_class_init             (DrawingApplicationClass *this_class);
static void drawing_application_class_init_application (GApplicationClass *this_class);
static void drawing_application_init                   (DrawingApplication *self);
static void drawing_application_init_accels            (GtkApplication *self);
static void drawing_application_open                   (GApplication *self, GFile **files, gint n_files, const gchar *hint);
static void drawing_application_startup                (GApplication *self);

/* Drawing Application クラス */
G_DEFINE_TYPE (DrawingApplication, drawing_application, GTK_TYPE_APPLICATION);
static const char *ACCELS_CLOSE        [] = { "<Ctrl>q", NULL };
static const char *ACCELS_HELP_OVERLAY [] = { "<Ctrl>question", "<Ctrl>slash", NULL };
static const char *ACCELS_NEW          [] = { "<Ctrl>n", NULL };
static const char *ACCELS_OPEN         [] = { "<Ctrl>o", NULL };

/* メニュー アクセラレーター */
static const DrawingApplicationAccelEntry
ACCEL_ENTRIES [] =
{
	{ "window.close",          ACCELS_CLOSE        },
	{ "win.show-help-overlay", ACCELS_HELP_OVERLAY },
	{ "app.new",               ACCELS_NEW          },
	{ "win.open",              ACCELS_OPEN         },
};

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ "new", drawing_application_activate_new, NULL, NULL, NULL },
};

/*******************************************************************************
アプリケーションを表示します。
*/
static void
drawing_application_activate (GApplication *self)
{
	GtkWidget *window;
	window = drawing_application_window_new (self);
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
新しいウィンドウを表示します。
*/
static void
drawing_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window;
	window = drawing_application_window_new (G_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
クラスを初期化します。
*/
static void
drawing_application_class_init (DrawingApplicationClass *this_class)
{
	drawing_application_class_init_application (G_APPLICATION_CLASS (this_class));
}

/*******************************************************************************
Application クラスを初期化します。
*/
static void
drawing_application_class_init_application (GApplicationClass *this_class)
{
	this_class->activate = drawing_application_activate;
	this_class->open = drawing_application_open;
	this_class->startup = drawing_application_startup;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
drawing_application_init (DrawingApplication *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
}

/*******************************************************************************
アクセラレーターを初期化します。
*/
static void
drawing_application_init_accels (GtkApplication *self)
{
	const DrawingApplicationAccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (self, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

/*******************************************************************************
クラスのインスタンスを作成します。
*/
GApplication *
drawing_application_new (const char *application_id, GApplicationFlags flags)
{
	return g_object_new (DRAWING_TYPE_APPLICATION,
		PROPERTY_APPLICATION_ID, application_id,
		PROPERTY_FLAGS, flags,
		NULL);
}

/*******************************************************************************
指定したファイルを開きます。
*/
static void
drawing_application_open (GApplication *self, GFile **files, gint n_files, const gchar *hint)
{
	GtkWidget *window;
	int n;

	for (n = 0; n < n_files; n++)
	{
		window = drawing_application_window_new (self);
		gtk_window_present (GTK_WINDOW (window));
	}
}

/*******************************************************************************
アプリケーションを開始します。
*/
static void
drawing_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (drawing_application_parent_class)->startup (self);
	drawing_application_init_accels (GTK_APPLICATION (self));
}
