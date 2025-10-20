/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_ABOUT "show-about"
#define ACTION_OPEN  "open"
#define PROPERTY_APPLICATION               "application"
#define PROPERTY_SHOW_MENUBAR              "show-menubar"
#define SIGNAL_DESTROY "destroy"
#define RESOURCE_ABOUT        "gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define RESOURCE_TEMPLATE     "viewerapplicationwindow.ui"
#define TITLE _("Picture Viewer")

/* Viewer Application Window クラスのインスタンス */
struct _ViewerApplicationWindow
{
	GtkApplicationWindow parent_instance;
	cairo_pattern_t     *pattern;
	cairo_surface_t     *surface;
	GFile               *file;
	GtkWidget           *area;
	float                background_red;
	float                background_green;
	float                background_blue;
};

static void     viewer_application_window_activate_about       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_open        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_class_init           (ViewerApplicationWindowClass *this_class);
static void     viewer_application_window_class_init_object    (GObjectClass *this_class);
static void     viewer_application_window_class_init_widget    (GtkWidgetClass *this_class);
static void     viewer_application_window_destroy              (ViewerApplicationWindow *self);
static void     viewer_application_window_dispose              (GObject *self);
static void     viewer_application_window_draw                 (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static gboolean viewer_application_window_get_background_equal (ViewerApplicationWindow *self, float red, float green, float blue);
static void     viewer_application_window_init                 (ViewerApplicationWindow *self);
static void     viewer_application_window_respond_open         (GObject *dialog, GAsyncResult *result, gpointer user_data);

/* Viewer Application Window クラス */
G_DEFINE_TYPE (ViewerApplicationWindow, viewer_application_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー項目アクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ ACTION_ABOUT, viewer_application_window_activate_about, NULL, NULL, NULL },
	{ ACTION_OPEN,  viewer_application_window_activate_open,  NULL, NULL, NULL },
};

/*******************************************************************************
バージョン情報ダイアログを表示します。
*/
static void
viewer_application_window_activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *builder;
	GtkWindow *dialog;
	char path [VIEWER_RESOURCE_PATH_CCH];
	viewer_get_resource_path (path, VIEWER_RESOURCE_PATH_CCH, RESOURCE_ABOUT);
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
ファイルを開きます。
*/
static void
viewer_application_window_activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();
	gtk_file_dialog_open (dialog, GTK_WINDOW (user_data), NULL, viewer_application_window_respond_open, user_data);
	g_object_unref (dialog);
}

/*******************************************************************************
クラスを初期化します。
*/
static void
viewer_application_window_class_init (ViewerApplicationWindowClass *this_class)
{
	viewer_application_window_class_init_object (G_OBJECT_CLASS (this_class));
	viewer_application_window_class_init_widget (GTK_WIDGET_CLASS (this_class));
}

/*******************************************************************************
Object クラスを初期化します。
*/
static void
viewer_application_window_class_init_object (GObjectClass *this_class)
{
	this_class->dispose = viewer_application_window_dispose;
}

/*******************************************************************************
Widget クラスを初期化します。
*/
static void
viewer_application_window_class_init_widget (GtkWidgetClass *this_class)
{
	char path [VIEWER_RESOURCE_PATH_CCH];
	viewer_get_resource_path (path, VIEWER_RESOURCE_PATH_CCH, RESOURCE_TEMPLATE);
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (this_class), path);
	gtk_widget_class_bind_template_child (this_class, ViewerApplicationWindow, area);
}

/*******************************************************************************
メンバー変数を破棄します。
*/
static void
viewer_application_window_destroy (ViewerApplicationWindow *self)
{
	g_clear_pointer (&self->pattern, cairo_pattern_destroy);
	g_clear_pointer (&self->surface, cairo_surface_destroy);
	g_clear_object (&self->file);
}

/*******************************************************************************
クラスのインスタンスを破棄します。
*/
static void
viewer_application_window_dispose (GObject *self)
{
	viewer_application_window_destroy (VIEWER_APPLICATION_WINDOW (self));
	gtk_widget_dispose_template (GTK_WIDGET (self), VIEWER_TYPE_APPLICATION_WINDOW);
	G_OBJECT_CLASS (viewer_application_window_parent_class)->dispose (self);
}

/*******************************************************************************
ウィンドウ領域を描画します。
*/
static void
viewer_application_window_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);

	if (!self->pattern)
	{
		self->pattern = cairo_pattern_create_rgb (self->background_red, self->background_green, self->background_blue);
	}
	if (!self->surface && self->file)
	{
		self->surface = viewer_create_surface_from_file (cairo, self->file, NULL);
	}
	if (self->pattern)
	{
		cairo_set_source (cairo, self->pattern);
		cairo_paint (cairo);
	}
	if (self->surface)
	{
		cairo_set_source_surface (cairo, self->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
現在の背景色を取得します。
*/
void
viewer_application_window_get_background (ViewerApplicationWindow *self, float *red, float *green, float *blue)
{
	*red = self->background_red;
	*green = self->background_green;
	*blue = self->background_blue;
}

/*******************************************************************************
現在の背景色を取得します。
*/
static gboolean
viewer_application_window_get_background_equal (ViewerApplicationWindow *self, float red, float green, float blue)
{
	return
		(self->background_red == red) &&
		(self->background_green == green) &&
		(self->background_blue == blue);
}

/*******************************************************************************
現在のファイルを取得します。
*/
GFile *
viewer_application_window_get_file (ViewerApplicationWindow *self)
{
	GFile *file;

	if (self->file)
	{
		file = g_object_ref (self->file);
	}
	else
	{
		file = NULL;
	}

	return file;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_window_init (ViewerApplicationWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_widget_init_template (GTK_WIDGET (self));
	gtk_window_set_title (GTK_WINDOW (self), TITLE);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self->area), viewer_application_window_draw, self, NULL);
	self->background_blue = 0.3F;
	self->background_green = 0.2F;
	self->background_red = 0.1F;
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

/*******************************************************************************
ファイルを開きます。
*/
static void
viewer_application_window_respond_open (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GFile *file;
	file = gtk_file_dialog_open_finish (GTK_FILE_DIALOG (dialog), result, NULL);

	if (file)
	{
		viewer_application_window_set_file (VIEWER_APPLICATION_WINDOW (user_data), file);
		g_object_unref (file);
	}
}

/*******************************************************************************
現在の背景色を設定します。
*/
void
viewer_application_window_set_background (ViewerApplicationWindow *self, float red, float green, float blue)
{
	if (!viewer_application_window_get_background_equal (self, red, green, blue))
	{
		self->background_red = red;
		self->background_green = green;
		self->background_blue = blue;
		g_clear_pointer (&self->pattern, cairo_pattern_destroy);
		gtk_widget_queue_draw (self->area);
	}
}

/*******************************************************************************
現在のファイルを設定します。
*/
void
viewer_application_window_set_file (ViewerApplicationWindow *self, GFile *file)
{
	if (self->file != file)
	{
		if (self->file)
		{
			g_object_unref (self->file);
		}
		if (file)
		{
			self->file = g_object_ref (file);
		}
		else
		{
			self->file = NULL;
		}

		g_clear_pointer (&self->surface, cairo_surface_destroy);
		gtk_widget_queue_draw (self->area);
	}
}
