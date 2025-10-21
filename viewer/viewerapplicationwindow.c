/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_ABOUT      "show-about"
#define ACTION_BACKGROUND "background"
#define ACTION_FULLSCREEN "fullscreen"
#define ACTION_OPEN       "open"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define SIGNAL_DESTROY      "destroy"
#define SIGNAL_NOTIFY_STATE "notify::state"
#define RESOURCE_ABOUT        "gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define RESOURCE_TEMPLATE     "viewerapplicationwindow.ui"
#define SETTINGS_FULLSCREEN "window-fullscreen"
#define SETTINGS_HEIGHT     "window-height"
#define SETTINGS_MAXIMIZED  "window-maximized"
#define SETTINGS_WIDTH      "window-width"
#define TITLE            _("Picture Viewer")
#define TITLE_BACKGROUND _("Background Color")
#define TITLE_OPEN       _("Open File")

/* Viewer Application Window クラスのプロパティ */
enum _ViewerApplicationWindowProperties
{
	NULL_PROPERTY_ID,
	BACKGROUND_BLUE_PROPERTY_ID,
	BACKGROUND_GREEN_PROPERTY_ID,
	BACKGROUND_RED_PROPERTY_ID,
	FILE_PROPERTY_ID,
	VIEWER_APPLICATION_WINDOW_N_PROPERTIES,
};

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
	int                  width;
	int                  height;
	unsigned char        fullscreen;
	unsigned char        maximized;
};

static void     viewer_application_window_activate_about       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_background  (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_fullscreen  (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_open        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_apply_settings       (ViewerApplicationWindow *self);
static void     viewer_application_window_class_init           (ViewerApplicationWindowClass *this_class);
static void     viewer_application_window_class_init_object    (GObjectClass *this_class);
static void     viewer_application_window_class_init_widget    (GtkWidgetClass *this_class);
static void     viewer_application_window_construct            (GObject *self);
static void     viewer_application_window_destroy              (ViewerApplicationWindow *self);
static void     viewer_application_window_dispose              (GObject *self);
static void     viewer_application_window_draw                 (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static gboolean viewer_application_window_get_background_equal (ViewerApplicationWindow *self, float red, float green, float blue);
static void     viewer_application_window_get_property         (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void     viewer_application_window_init                 (ViewerApplicationWindow *self);
static void     viewer_application_window_load_settings        (ViewerApplicationWindow *self);
static void     viewer_application_window_realize              (GtkWidget *self);
static void     viewer_application_window_resize               (GtkWidget *self, int width, int height, int baseline);
static void     viewer_application_window_respond_background   (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     viewer_application_window_respond_open         (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     viewer_application_window_save_settings        (ViewerApplicationWindow *self);
static void     viewer_application_window_set_property         (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void     viewer_application_window_unrealize            (GtkWidget *self);
static void     viewer_application_window_update_size          (ViewerApplicationWindow *self);
static void     viewer_application_window_update_surface       (GObject *object, GParamSpec *pspec, gpointer user_data);

/* Viewer Application Window クラス */
G_DEFINE_TYPE (ViewerApplicationWindow, viewer_application_window, GTK_TYPE_APPLICATION_WINDOW);

/* Background Blue プロパティ */
#define BACKGROUND_BLUE_PROPERTY_NAME           "background-blue"
#define BACKGROUND_BLUE_PROPERTY_NICK           "Background Blue"
#define BACKGROUND_BLUE_PROPERTY_BLURB          "Background Blue"
#define BACKGROUND_BLUE_PROPERTY_MINIMUM_VALUE  0.0F
#define BACKGROUND_BLUE_PROPERTY_MAXIMUM_VALUE  1.0F
#define BACKGROUND_BLUE_PROPERTY_DEFAULT_VALUE  0.3F
#define BACKGROUND_BLUE_PROPERTY_FLAGS          G_PARAM_READWRITE

/* Background Green プロパティ */
#define BACKGROUND_GREEN_PROPERTY_NAME          "background-green"
#define BACKGROUND_GREEN_PROPERTY_NICK          "Background Green"
#define BACKGROUND_GREEN_PROPERTY_BLURB         "Background Green"
#define BACKGROUND_GREEN_PROPERTY_MINIMUM_VALUE 0.0F
#define BACKGROUND_GREEN_PROPERTY_MAXIMUM_VALUE 1.0F
#define BACKGROUND_GREEN_PROPERTY_DEFAULT_VALUE 0.2F
#define BACKGROUND_GREEN_PROPERTY_FLAGS         G_PARAM_READWRITE

/* Background Red プロパティ */
#define BACKGROUND_RED_PROPERTY_NAME          "background-red"
#define BACKGROUND_RED_PROPERTY_NICK          "Background Red"
#define BACKGROUND_RED_PROPERTY_BLURB         "Background Red"
#define BACKGROUND_RED_PROPERTY_MINIMUM_VALUE 0.0F
#define BACKGROUND_RED_PROPERTY_MAXIMUM_VALUE 1.0F
#define BACKGROUND_RED_PROPERTY_DEFAULT_VALUE 0.1F
#define BACKGROUND_RED_PROPERTY_FLAGS         G_PARAM_READWRITE

/* Background File プロパティ */
#define FILE_PROPERTY_NAME        "file"
#define FILE_PROPERTY_NICK        "File"
#define FILE_PROPERTY_BLURB       "File"
#define FILE_PROPERTY_OBJECT_TYPE G_TYPE_FILE
#define FILE_PROPERTY_FLAGS       G_PARAM_READWRITE

/* メニュー項目アクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ ACTION_ABOUT,      viewer_application_window_activate_about,      NULL, NULL, NULL },
	{ ACTION_BACKGROUND, viewer_application_window_activate_background, NULL, NULL, NULL },
	{ ACTION_FULLSCREEN, viewer_application_window_activate_fullscreen, NULL, NULL, NULL },
	{ ACTION_OPEN,       viewer_application_window_activate_open,       NULL, NULL, NULL },
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
背景色を選択します。
*/
static void
viewer_application_window_activate_background (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerApplicationWindow *self;
	GtkColorDialog *dialog;
	GdkRGBA color;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	color.red   = self->background_red;
	color.green = self->background_green;
	color.blue  = self->background_blue;
	color.alpha = 0.0F;
	dialog = gtk_color_dialog_new   ();
	gtk_color_dialog_set_modal      (dialog, TRUE);
	gtk_color_dialog_set_title      (dialog, TITLE_BACKGROUND);
	gtk_color_dialog_set_with_alpha (dialog, FALSE);
	gtk_color_dialog_choose_rgba    (dialog, GTK_WINDOW (user_data), &color, NULL, viewer_application_window_respond_background, user_data);
	g_object_unref                  (dialog);
}

/*******************************************************************************
ウィンドウを全画面表示します。
*/
static void
viewer_application_window_activate_fullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	if (VIEWER_APPLICATION_WINDOW (user_data)->fullscreen)
	{
		gtk_window_unfullscreen (GTK_WINDOW (user_data));
	}
	else
	{
		gtk_window_fullscreen (GTK_WINDOW (user_data));
	}
}

/*******************************************************************************
ファイルを開きます。
*/
static void
viewer_application_window_activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkFileDialog *dialog;
	dialog = gtk_file_dialog_new ();
	gtk_file_dialog_set_modal    (dialog, TRUE);
	gtk_file_dialog_set_title    (dialog, TITLE_OPEN);
	gtk_file_dialog_open         (dialog, GTK_WINDOW (user_data), NULL, viewer_application_window_respond_open, user_data);
	g_object_unref               (dialog);
}

/*******************************************************************************
環境設定を適用します。
*/
static void
viewer_application_window_apply_settings (ViewerApplicationWindow *self)
{
	GtkWindow *window;
	window = GTK_WINDOW (self);
	gtk_window_set_default_size (window, self->width, self->height);

	if (self->maximized)
	{
		gtk_window_maximize (window);
	}
	if (self->fullscreen)
	{
		gtk_window_fullscreen (window);
	}
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
	GParamSpec *pspecs [VIEWER_APPLICATION_WINDOW_N_PROPERTIES] = { NULL };
	pspecs [BACKGROUND_BLUE_PROPERTY_ID]  = PARAM_SPEC_FLOAT  (BACKGROUND_BLUE_PROPERTY);
	pspecs [BACKGROUND_GREEN_PROPERTY_ID] = PARAM_SPEC_FLOAT  (BACKGROUND_GREEN_PROPERTY);
	pspecs [BACKGROUND_RED_PROPERTY_ID]   = PARAM_SPEC_FLOAT  (BACKGROUND_RED_PROPERTY);
	pspecs [FILE_PROPERTY_ID]             = PARAM_SPEC_OBJECT (FILE_PROPERTY);
	this_class->constructed  = viewer_application_window_construct;
	this_class->dispose      = viewer_application_window_dispose;
	this_class->get_property = viewer_application_window_get_property;
	this_class->set_property = viewer_application_window_set_property;
	g_object_class_install_properties (this_class, G_N_ELEMENTS (pspecs), pspecs);
}

/*******************************************************************************
Widget クラスを初期化します。
*/
static void
viewer_application_window_class_init_widget (GtkWidgetClass *this_class)
{
	char path [VIEWER_RESOURCE_PATH_CCH];
	this_class->realize       = viewer_application_window_realize;
	this_class->size_allocate = viewer_application_window_resize;
	this_class->unrealize     = viewer_application_window_unrealize;
	viewer_get_resource_path (path, VIEWER_RESOURCE_PATH_CCH, RESOURCE_TEMPLATE);
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (this_class), path);
	gtk_widget_class_bind_template_child (this_class, ViewerApplicationWindow, area);
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_window_construct (GObject *self)
{
	viewer_application_window_load_settings (VIEWER_APPLICATION_WINDOW (self));
	viewer_application_window_apply_settings (VIEWER_APPLICATION_WINDOW (self));
	G_OBJECT_CLASS (viewer_application_window_parent_class)->constructed (self);
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
	viewer_application_window_save_settings (VIEWER_APPLICATION_WINDOW (self));
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
プロパティを取得します。
*/
static void
viewer_application_window_get_property (GObject *self, guint property_id, GValue *value, GParamSpec *pspec)
{
	ViewerApplicationWindow *properties;
	properties = VIEWER_APPLICATION_WINDOW (self);

	switch (property_id)
	{
	case BACKGROUND_BLUE_PROPERTY_ID:
		g_value_set_float (value, properties->background_blue);
		break;
	case BACKGROUND_GREEN_PROPERTY_ID:
		g_value_set_float (value, properties->background_green);
		break;
	case BACKGROUND_RED_PROPERTY_ID:
		g_value_set_float (value, properties->background_red);
		break;
	case FILE_PROPERTY_ID:
		g_value_set_object (value, properties->file);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_window_init (ViewerApplicationWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_widget_init_template        (GTK_WIDGET (self));
	gtk_window_set_title            (GTK_WINDOW (self), TITLE);
	gtk_drawing_area_set_draw_func  (GTK_DRAWING_AREA (self->area), viewer_application_window_draw, self, NULL);
	self->background_blue  = BACKGROUND_BLUE_PROPERTY_DEFAULT_VALUE;
	self->background_green = BACKGROUND_GREEN_PROPERTY_DEFAULT_VALUE;
	self->background_red   = BACKGROUND_RED_PROPERTY_DEFAULT_VALUE;
}

/*******************************************************************************
環境設定を開きます。
*/
static void
viewer_application_window_load_settings (ViewerApplicationWindow *self)
{
	GSettings *settings;
	settings         = viewer_get_settings    ();
	self->width      = g_settings_get_int     (settings, SETTINGS_WIDTH);
	self->height     = g_settings_get_int     (settings, SETTINGS_HEIGHT);
	self->fullscreen = g_settings_get_boolean (settings, SETTINGS_FULLSCREEN);
	self->maximized  = g_settings_get_boolean (settings, SETTINGS_MAXIMIZED);
	g_object_unref (settings);
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
ウィンドウを表示します。
*/
static void
viewer_application_window_realize (GtkWidget *self)
{
	GTK_WIDGET_CLASS (viewer_application_window_parent_class)->realize (self);
	g_signal_connect (gtk_native_get_surface (GTK_NATIVE (self)), SIGNAL_NOTIFY_STATE, G_CALLBACK (viewer_application_window_update_surface), self);
}

/*******************************************************************************
ウィンドウの大きさを変更します。
*/
static void
viewer_application_window_resize (GtkWidget *self, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (viewer_application_window_parent_class)->size_allocate (self, width, height, baseline);
	viewer_application_window_update_size (VIEWER_APPLICATION_WINDOW (self));
}

/*******************************************************************************
背景色を選択します。
*/
static void
viewer_application_window_respond_background (GObject *dialog, GAsyncResult *result, gpointer user_data)
{
	GdkRGBA *color;
	color = gtk_color_dialog_choose_rgba_finish (GTK_COLOR_DIALOG (dialog), result, NULL);

	if (color)
	{
		viewer_application_window_set_background (VIEWER_APPLICATION_WINDOW (user_data), color->red, color->green, color->blue);
		g_free (color);
	}
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
環境設定を保存します。
*/
static void
viewer_application_window_save_settings (ViewerApplicationWindow *self)
{
	GSettings *settings;
	settings = viewer_get_settings ();
	g_settings_set_int (settings, SETTINGS_WIDTH, self->width);
	g_settings_set_int (settings, SETTINGS_HEIGHT, self->height);
	g_settings_set_boolean (settings, SETTINGS_FULLSCREEN, self->fullscreen);
	g_settings_set_boolean (settings, SETTINGS_MAXIMIZED, self->maximized);
	g_object_unref (settings);
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

/*******************************************************************************
プロパティを設定します。
*/
static void
viewer_application_window_set_property (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec)
{
	ViewerApplicationWindow *properties;
	properties = VIEWER_APPLICATION_WINDOW (self);

	switch (property_id)
	{
	case BACKGROUND_BLUE_PROPERTY_ID:
		viewer_application_window_set_background (properties, properties->background_red, properties->background_green, g_value_get_float (value));
		break;
	case BACKGROUND_GREEN_PROPERTY_ID:
		viewer_application_window_set_background (properties, properties->background_red, g_value_get_float (value), properties->background_blue);
		break;
	case BACKGROUND_RED_PROPERTY_ID:
		viewer_application_window_set_background (properties, g_value_get_float (value), properties->background_green, properties->background_blue);
		break;
	case FILE_PROPERTY_ID:
		viewer_application_window_set_file (properties, g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
ウィンドウを隠蔽します。
*/
static void
viewer_application_window_unrealize (GtkWidget *self)
{
	g_signal_handlers_disconnect_by_func (gtk_native_get_surface (GTK_NATIVE (self)), viewer_application_window_update_surface, self);
	GTK_WIDGET_CLASS (viewer_application_window_parent_class)->unrealize (self);
}

/*******************************************************************************
現在の大きさを更新します。
*/
static void
viewer_application_window_update_size (ViewerApplicationWindow *self)
{
	if (!self->maximized && !self->fullscreen)
	{
		gtk_window_get_default_size (GTK_WINDOW (self), &self->width, &self->height);
	}
}

/*******************************************************************************
ウィンドウの大きさを変更します。
*/
static void
viewer_application_window_update_surface (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	ViewerApplicationWindow *self;
	GdkSurface *surface;
	GdkToplevelState state;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	surface = GDK_SURFACE (object);
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (surface));
	self->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
	self->fullscreen = (state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
}
