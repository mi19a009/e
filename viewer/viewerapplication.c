/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define ACTION_NEW              "new"
#define ATTRIBUTE_ACCEL         "accel"
#define ATTRIBUTE_ACTION        "action"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"

typedef struct _ViewerApplicationAccelEntry ViewerApplicationAccelEntry;

/* Viewer Application クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
};

struct _ViewerApplicationAccelEntry
{
	const char  *detailed_action_name;
	const char **accels;
};

static void viewer_application_activate                   (GApplication *self);
static void viewer_application_activate_new               (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_application_class_init                 (ViewerApplicationClass *self);
static void viewer_application_class_init_application     (GApplicationClass *self);
static void viewer_application_init                       (ViewerApplication *self);
static void viewer_application_init_accels                (GtkApplication *self);
static void viewer_application_open                       (GApplication *self, GFile **files, int n_files, const char *hint);
static void viewer_application_startup                    (GApplication *self);

/* Viewer Application クラス */
G_DEFINE_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
static const char *ACCELS_BACKGROUND   [] = { "F12", NULL };
static const char *ACCELS_CLOSE        [] = { "<Ctrl>q", NULL };
static const char *ACCELS_FULLSCREEN   [] = { "F11", NULL };
static const char *ACCELS_HELP_OVERLAY [] = { "<Ctrl>question", "<Ctrl>slash", NULL };
static const char *ACCELS_NEW          [] = { "<Ctrl>n", NULL };
static const char *ACCELS_OPEN         [] = { "<Ctrl>o", NULL };
static const char *ACCELS_RESTORE_ZOOM [] = { "<Ctrl>0", NULL };
static const char *ACCELS_ZOOM_IN      [] = { "<Ctrl>plus", "<Ctrl>semicolon", NULL };
static const char *ACCELS_ZOOM_OUT     [] = { "<Ctrl>minus", NULL };

/* メニュー アクセラレーター */
static const ViewerApplicationAccelEntry
ACCEL_ENTRIES [] =
{
	{ "win.background",        ACCELS_BACKGROUND   },
	{ "window.close",          ACCELS_CLOSE        },
	{ "win.fullscreen",        ACCELS_FULLSCREEN   },
	{ "win.show-help-overlay", ACCELS_HELP_OVERLAY },
	{ "app.new",               ACCELS_NEW          },
	{ "win.open",              ACCELS_OPEN         },
	{ "win.restore-zoom",      ACCELS_RESTORE_ZOOM },
	{ "win.zoom-in",           ACCELS_ZOOM_IN      },
	{ "win.zoom-out",          ACCELS_ZOOM_OUT     },
};

/* メニュー アクション */
static const GActionEntry
ACTION_ENTRIES [] =
{
	{ ACTION_NEW, viewer_application_activate_new, NULL, NULL, NULL },
};

/*******************************************************************************
アプリケーションを表示します。
*/
static void
viewer_application_activate (GApplication *self)
{
	GtkWidget *window;
	window = viewer_application_window_new (self);
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
新しいウィンドウを表示します。
*/
static void
viewer_application_activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window;
	window = viewer_application_window_new (G_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (window));
}

/*******************************************************************************
クラスを初期化します。
*/
static void
viewer_application_class_init (ViewerApplicationClass *self)
{
	viewer_application_class_init_application (G_APPLICATION_CLASS (self));
}

/*******************************************************************************
Application クラスを初期化します。
*/
static void
viewer_application_class_init_application (GApplicationClass *self)
{
	self->activate = viewer_application_activate;
	self->open = viewer_application_open;
	self->startup = viewer_application_startup;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_init (ViewerApplication *self)
{
}

/*******************************************************************************
アクセラレーターを初期化します。
*/
static void
viewer_application_init_accels (GtkApplication *self)
{
	const ViewerApplicationAccelEntry *entries;
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
viewer_application_new (const char *application_id, GApplicationFlags flags)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		PROPERTY_APPLICATION_ID, application_id,
		PROPERTY_FLAGS, flags,
		NULL);
}

/*******************************************************************************
指定したファイルを開きます。
*/
static void
viewer_application_open (GApplication *self, GFile **files, int n_files, const char *hint)
{
	GtkWidget *window;
	int n;

	for (n = 0; n < n_files; n++)
	{
		window = viewer_application_window_new (self);
		gtk_window_present (GTK_WINDOW (window));
	}
}

/*******************************************************************************
アプリケーションを開始します。
*/
static void
viewer_application_startup (GApplication *self)
{
	G_APPLICATION_CLASS (viewer_application_parent_class)->startup (self);
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	viewer_application_init_accels (GTK_APPLICATION (self));
}
