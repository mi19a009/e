/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define PROPERTY_APPLICATION               "application"
#define PROPERTY_SHOW_MENUBAR              "show-menubar"
#define TITLE _("Picture Viewer")

/* Viewer Application Window クラスのインスタンス */
struct _ViewerApplicationWindow
{
	GtkApplicationWindow parent_instance;
};

static void viewer_application_window_class_init        (ViewerApplicationWindowClass *self);
static void viewer_application_window_init              (ViewerApplicationWindow *self);

/* Viewer Application Window クラス */
G_DEFINE_TYPE (ViewerApplicationWindow, viewer_application_window, GTK_TYPE_APPLICATION_WINDOW);

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
