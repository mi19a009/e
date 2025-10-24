/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_ABOUT          "show-about"
#define ACTION_BACKGROUND     "background"
#define ACTION_FULLSCREEN     "fullscreen"
#define ACTION_OPEN           "open"
#define ACTION_RESTORE_ZOOM   "restore-zoom"
#define ACTION_ZOOM_IN        "zoom-in"
#define ACTION_ZOOM_OUT       "zoom-out"
#define FORMAT_TITLE          "%s - %s"
#define FORMAT_ZOOM_TITLE     "%.0f%% %s - %s"
#define PROPERTY_APPLICATION  "application"
#define PROPERTY_SHOW_MENUBAR "show-menubar"
#define RESOURCE_ABOUT        "gtk/about.ui"
#define RESOURCE_ABOUT_DIALOG "dialog"
#define RESOURCE_TEMPLATE     "viewerapplicationwindow.ui"
#define SETTINGS_FULLSCREEN   "window-fullscreen"
#define SETTINGS_HEIGHT       "window-height"
#define SETTINGS_MAXIMIZED    "window-maximized"
#define SETTINGS_WIDTH        "window-width"
#define SIGNAL_BEGIN          "begin"
#define SIGNAL_DESTROY        "destroy"
#define SIGNAL_END            "end"
#define SIGNAL_NOTIFY_STATE   "notify::state"
#define SIGNAL_SCALE_CHANGED  "scale-changed"
#define TITLE                 _("Picture Viewer")
#define TITLE_BACKGROUND      _("Background Color")
#define TITLE_CCH             256
#define TITLE_OPEN            _("Open File")
#define ZOOM_INCREMENT        1.25F

/* Viewer Application Window クラスのプロパティ */
enum _ViewerApplicationWindowProperties
{
	NULL_PROPERTY_ID,
	BACKGROUND_BLUE_PROPERTY_ID,
	BACKGROUND_GREEN_PROPERTY_ID,
	BACKGROUND_RED_PROPERTY_ID,
	FILE_PROPERTY_ID,
	ZOOM_PROPERTY_ID,
	VIEWER_APPLICATION_WINDOW_N_PROPERTIES,
};

/* Viewer Application Window クラスのインスタンス */
struct _ViewerApplicationWindow
{
	GtkApplicationWindow parent_instance;
	char                *name;
	cairo_pattern_t     *pattern;
	cairo_surface_t     *surface;
	GFile               *file;
	GtkAdjustment       *hadjustment;
	GtkAdjustment       *vadjustment;
	GtkWidget           *area;
	float                background_red;
	float                background_green;
	float                background_blue;
	float                zoom;
	float                zoom_origin;
	int                  area_width;
	int                  area_height;
	int                  surface_width;
	int                  surface_height;
	int                  width;
	int                  height;
	unsigned char        fullscreen;
	unsigned char        maximized;
};

static void     viewer_application_window_activate_about        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_background   (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_fullscreen   (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_open         (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_restore_zoom (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_zoom_in      (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_activate_zoom_out     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     viewer_application_window_apply_settings        (ViewerApplicationWindow *self);
static void     viewer_application_window_begin_zoom            (GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data);
static void     viewer_application_window_change_adjustment     (GtkAdjustment *adjustment, gpointer user_data);
static void     viewer_application_window_change_zoom           (GtkGestureZoom *gesture, gdouble delta, gpointer user_data);
static void     viewer_application_window_class_init            (ViewerApplicationWindowClass *this_class);
static void     viewer_application_window_class_init_object     (GObjectClass *this_class);
static void     viewer_application_window_class_init_widget     (GtkWidgetClass *this_class);
static void     viewer_application_window_construct             (GObject *self);
static void     viewer_application_window_destroy               (ViewerApplicationWindow *self);
static void     viewer_application_window_dispose               (GObject *self);
static void     viewer_application_window_draw                  (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void     viewer_application_window_end_zoom              (GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data);
static gboolean viewer_application_window_get_background_equal  (ViewerApplicationWindow *self, float red, float green, float blue);
static void     viewer_application_window_get_property          (GObject *self, guint property_id, GValue *value, GParamSpec *pspec);
static void     viewer_application_window_init                  (ViewerApplicationWindow *self);
static void     viewer_application_window_init_gestures         (ViewerApplicationWindow *self);
static void     viewer_application_window_load_settings         (ViewerApplicationWindow *self);
static void     viewer_application_window_realize               (GtkWidget *self);
static void     viewer_application_window_resize                (GtkWidget *self, int width, int height, int baseline);
static void     viewer_application_window_resize_area           (GtkDrawingArea *area, int width, int height, gpointer user_data);
static void     viewer_application_window_respond_background    (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     viewer_application_window_respond_open          (GObject *dialog, GAsyncResult *result, gpointer user_data);
static void     viewer_application_window_save_settings         (ViewerApplicationWindow *self);
static void     viewer_application_window_set_property          (GObject *self, guint property_id, const GValue *value, GParamSpec *pspec);
static void     viewer_application_window_unrealize             (GtkWidget *self);
static void     viewer_application_window_update_name           (ViewerApplicationWindow *self);
static void     viewer_application_window_update_range          (ViewerApplicationWindow *self);
static void     viewer_application_window_update_size           (ViewerApplicationWindow *self);
static void     viewer_application_window_update_surface        (GObject *object, GParamSpec *pspec, gpointer user_data);
static void     viewer_application_window_update_title          (ViewerApplicationWindow *self);

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

/* File プロパティ */
#define FILE_PROPERTY_NAME        "file"
#define FILE_PROPERTY_NICK        "File"
#define FILE_PROPERTY_BLURB       "File"
#define FILE_PROPERTY_OBJECT_TYPE G_TYPE_FILE
#define FILE_PROPERTY_FLAGS       G_PARAM_READWRITE

/* Zoom プロパティ */
#define ZOOM_PROPERTY_NAME          "zoom"
#define ZOOM_PROPERTY_NICK          "Zoom"
#define ZOOM_PROPERTY_BLURB         "Zoom"
#define ZOOM_PROPERTY_MINIMUM_VALUE FLT_MIN
#define ZOOM_PROPERTY_MAXIMUM_VALUE FLT_MAX
#define ZOOM_PROPERTY_DEFAULT_VALUE 1.0F
#define ZOOM_PROPERTY_FLAGS         G_PARAM_READWRITE

/* メニュー項目アクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ ACTION_ABOUT,        viewer_application_window_activate_about,        NULL, NULL, NULL },
	{ ACTION_BACKGROUND,   viewer_application_window_activate_background,   NULL, NULL, NULL },
	{ ACTION_FULLSCREEN,   viewer_application_window_activate_fullscreen,   NULL, NULL, NULL },
	{ ACTION_OPEN,         viewer_application_window_activate_open,         NULL, NULL, NULL },
	{ ACTION_RESTORE_ZOOM, viewer_application_window_activate_restore_zoom, NULL, NULL, NULL },
	{ ACTION_ZOOM_IN,      viewer_application_window_activate_zoom_in,      NULL, NULL, NULL },
	{ ACTION_ZOOM_OUT,     viewer_application_window_activate_zoom_out,     NULL, NULL, NULL },
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
既定の拡大率に戻します。
*/
static void
viewer_application_window_activate_restore_zoom (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	viewer_application_window_set_zoom (VIEWER_APPLICATION_WINDOW (user_data), ZOOM_PROPERTY_DEFAULT_VALUE);
}

/*******************************************************************************
詳細表示します。
*/
static void
viewer_application_window_activate_zoom_in (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	viewer_application_window_set_zoom (self, self->zoom * ZOOM_INCREMENT);
}

/*******************************************************************************
広域表示します。
*/
static void
viewer_application_window_activate_zoom_out (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	viewer_application_window_set_zoom (self, self->zoom * 1.0F / ZOOM_INCREMENT);
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
拡大を開始します。
*/
static void
viewer_application_window_begin_zoom (GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	self->zoom_origin = self->zoom;
}

/*******************************************************************************
スクロール位置を変更します。
*/
static void
viewer_application_window_change_adjustment (GtkAdjustment *adjustment, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	gtk_widget_queue_draw (self->area);
}

/*******************************************************************************
拡大率を変更します。
*/
static void
viewer_application_window_change_zoom (GtkGestureZoom *gesture, gdouble delta, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	viewer_application_window_set_zoom (self, self->zoom_origin * delta);
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
	pspecs [ZOOM_PROPERTY_ID]             = PARAM_SPEC_FLOAT  (ZOOM_PROPERTY);
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
	gtk_widget_class_bind_template_child (this_class, ViewerApplicationWindow, hadjustment);
	gtk_widget_class_bind_template_child (this_class, ViewerApplicationWindow, vadjustment);
	gtk_widget_class_bind_template_child (this_class, ViewerApplicationWindow, area);
	gtk_widget_class_bind_template_callback (this_class, viewer_application_window_change_adjustment);
	gtk_widget_class_bind_template_callback (this_class, viewer_application_window_resize_area);
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
	g_clear_pointer (&self->name, g_free);
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
	double zoom;
	self = VIEWER_APPLICATION_WINDOW (user_data);

	if (!self->pattern)
	{
		self->pattern = cairo_pattern_create_rgb (self->background_red, self->background_green, self->background_blue);
	}
	if (!self->surface && self->file)
	{
		self->surface = viewer_create_surface_from_file (cairo, self->file, &self->surface_width, &self->surface_height, NULL);
		g_idle_add_once ((GSourceOnceFunc) viewer_application_window_update_range, self);
	}
	if (self->pattern)
	{
		cairo_set_source (cairo, self->pattern);
		cairo_paint (cairo);
	}
	if (self->surface)
	{
		zoom = self->zoom;
		cairo_translate (cairo, -gtk_adjustment_get_value (self->hadjustment), -gtk_adjustment_get_value (self->vadjustment));
		cairo_scale (cairo, zoom, zoom);
		cairo_set_source_surface (cairo, self->surface, 0, 0);
		cairo_paint (cairo);
	}
}

/*******************************************************************************
拡大を終了します。
*/
static void
viewer_application_window_end_zoom (GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	self->zoom_origin = 0;
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
	case ZOOM_PROPERTY_ID:
		g_value_set_float (value, properties->zoom);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
現在の拡大率を取得します。
*/
float
viewer_application_window_get_zoom (ViewerApplicationWindow *self)
{
	return self->zoom;
}

/*******************************************************************************
クラスのインスタンスを初期化します。
*/
static void
viewer_application_window_init (ViewerApplicationWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_widget_init_template        (GTK_WIDGET (self));
	gtk_drawing_area_set_draw_func  (GTK_DRAWING_AREA (self->area), viewer_application_window_draw, self, NULL);
	self->background_blue  = BACKGROUND_BLUE_PROPERTY_DEFAULT_VALUE;
	self->background_green = BACKGROUND_GREEN_PROPERTY_DEFAULT_VALUE;
	self->background_red   = BACKGROUND_RED_PROPERTY_DEFAULT_VALUE;
	self->zoom             = ZOOM_PROPERTY_DEFAULT_VALUE;
	viewer_application_window_init_gestures (self);
	viewer_application_window_update_title (self);
}

/*******************************************************************************
ジェスチャを追加します。
*/
static void
viewer_application_window_init_gestures (ViewerApplicationWindow *self)
{
	GtkGesture *gesture;
	gesture = gtk_gesture_zoom_new ();
	g_signal_connect (gesture, SIGNAL_BEGIN,         G_CALLBACK (viewer_application_window_begin_zoom),  self);
	g_signal_connect (gesture, SIGNAL_END,           G_CALLBACK (viewer_application_window_end_zoom),    self);
	g_signal_connect (gesture, SIGNAL_SCALE_CHANGED, G_CALLBACK (viewer_application_window_change_zoom), self);
	gtk_widget_add_controller (self->area, GTK_EVENT_CONTROLLER (gesture));
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
描画領域の大きさを変更します。
*/
static void
viewer_application_window_resize_area (GtkDrawingArea *area, int width, int height, gpointer user_data)
{
	ViewerApplicationWindow *self;
	self = VIEWER_APPLICATION_WINDOW (user_data);
	self->area_width = width;
	self->area_height = height;
	viewer_application_window_update_range (VIEWER_APPLICATION_WINDOW (self));
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
		viewer_application_window_update_name (self);
		viewer_application_window_update_title (self);
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
	case ZOOM_PROPERTY_ID:
		viewer_application_window_set_zoom (properties, g_value_get_float (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
		break;
	}
}

/*******************************************************************************
現在の拡大率を設定します。
*/
void
viewer_application_window_set_zoom (ViewerApplicationWindow *self, float zoom)
{
	if (self->zoom != zoom)
	{
		self->zoom = zoom;
		gtk_widget_queue_draw (self->area);
		viewer_application_window_update_range (VIEWER_APPLICATION_WINDOW (self));
		viewer_application_window_update_title (self);
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
開いたファイルの名前を更新します。
*/
static void
viewer_application_window_update_name (ViewerApplicationWindow *self)
{
	if (self->name)
	{
		g_free (self->name);
	}
	if (self->file)
	{
		self->name = g_file_get_basename (self->file);
	}
	else
	{
		self->name = NULL;
	}
}

/*******************************************************************************
スクロール範囲を更新します。
*/
static void
viewer_application_window_update_range (ViewerApplicationWindow *self)
{
	gtk_adjustment_set_page_size (self->hadjustment, self->area_width * self->zoom);
	gtk_adjustment_set_upper     (self->hadjustment, self->surface_width);
	gtk_adjustment_set_page_size (self->vadjustment, self->area_height * self->zoom);
	gtk_adjustment_set_upper     (self->vadjustment, self->surface_height);
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

/*******************************************************************************
ウィンドウ タイトルを更新します。
*/
static void
viewer_application_window_update_title (ViewerApplicationWindow *self)
{
	const char *title;
	char buffer [TITLE_CCH];

	if (self->name)
	{
		title = buffer;

		if (self->zoom == ZOOM_PROPERTY_DEFAULT_VALUE)
		{
			g_snprintf (buffer, TITLE_CCH, FORMAT_TITLE, self->name, TITLE);
		}
		else
		{
			g_snprintf (buffer, TITLE_CCH, FORMAT_ZOOM_TITLE, self->zoom * 100.0F, self->name, TITLE);
		}
	}
	else
	{
		title = TITLE;
	}

	gtk_window_set_title (GTK_WINDOW (self), title);
}
