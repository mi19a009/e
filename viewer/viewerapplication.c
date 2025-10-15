/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define ACTION_NEW              "new"
#define ATTRIBUTE_ACCEL         "accel"
#define ATTRIBUTE_ACTION        "action"
#define PROPERTY_APPLICATION_ID "application-id"
#define PROPERTY_FLAGS          "flags"

/* Viewer Application クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
};

static void viewer_application_activate                   (GApplication *self);
static void viewer_application_activate_new               (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void viewer_application_class_init                 (ViewerApplicationClass *self);
static void viewer_application_class_init_application     (GApplicationClass *self);
static void viewer_application_init                       (ViewerApplication *self);
static void viewer_application_init_accels                (ViewerApplication *self);
static void viewer_application_init_accels_for_attributes (GtkApplication *self, GMenuModel *model);
static void viewer_application_init_accels_for_links      (GtkApplication *self, GMenuModel *model);
static void viewer_application_open                       (GApplication *self, GFile **files, int n_files, const char *hint);
static void viewer_application_startup                    (GApplication *self);

/* Viewer Application クラス */
G_DEFINE_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);

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
メニュー バーからアクセラレーターを取得します。
*/
static void
viewer_application_init_accels (ViewerApplication *self)
{
	GMenuModel *menubar;
	menubar = gtk_application_get_menubar (GTK_APPLICATION (self));

	if (menubar)
	{
		viewer_application_init_accels_for_links (GTK_APPLICATION (self), menubar);
		g_object_unref (menubar);
	}
}

/*******************************************************************************
アクセラレーターを初期化します。
指定したメニューの属性を列挙します。
アプリケーションにアクセラレーターを設定します。
*/
static void
viewer_application_init_accels_for_attributes (GtkApplication *self, GMenuModel *model)
{
	GVariant *accel, *action;
	const char *name, *accels [2];
	int n, n_items;
	accels [1] = NULL;
	n_items = g_menu_model_get_n_items (model);

	for (n = 0; n < n_items; n++)
	{
		accel = g_menu_model_get_item_attribute_value (model, n, ATTRIBUTE_ACCEL, G_VARIANT_TYPE_STRING);

		if (accel)
		{
			action = g_menu_model_get_item_attribute_value (model, n, ATTRIBUTE_ACTION, G_VARIANT_TYPE_STRING);

			if (action)
			{
				name = g_variant_get_string (action, NULL);
				accels [0] = g_variant_get_string (accel, NULL);
				gtk_application_set_accels_for_action (self, name, accels);
				g_variant_unref (action);
			}

			g_variant_unref (accel);
		}
	}
}

/*******************************************************************************
アクセラレーターを初期化します。
指定したメニューのリンクを列挙します。
*/
static void
viewer_application_init_accels_for_links (GtkApplication *self, GMenuModel *model)
{
	GMenuLinkIter *links;
	GMenuModel *link;
	int n, n_items;
	n_items = g_menu_model_get_n_items (model);

	for (n = 0; n < n_items; n++)
	{
		links = g_menu_model_iterate_item_links (model, n);

		while (g_menu_link_iter_next (links))
		{
			link = g_menu_link_iter_get_value (links);
			viewer_application_init_accels_for_attributes (self, link);
			viewer_application_init_accels_for_links (self, link);
			g_object_unref (link);
		}

		g_object_unref (links);
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
	viewer_application_init_accels (VIEWER_APPLICATION (self));
}
