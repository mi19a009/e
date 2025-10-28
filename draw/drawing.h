/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define DRAWING_RESOURCE_PATH_CCH 64
#define DRAWING_TYPE_APPLICATION        (drawing_application_get_type        ())
#define DRAWING_TYPE_APPLICATION_WINDOW (drawing_application_window_get_type ())
#define DRAWING_TYPE_CIRCLE             (drawing_circle_get_type             ())
#define DRAWING_TYPE_CLUSTER            (drawing_cluster_get_type            ())
#define DRAWING_TYPE_DOCUMENT           (drawing_document_get_type           ())
#define DRAWING_TYPE_ELLIPSE            (drawing_ellipse_get_type            ())
#define DRAWING_TYPE_RECTANGLE          (drawing_rectangle_get_type          ())
#define DRAWING_TYPE_SHAPE              (drawing_shape_get_type              ())

typedef struct _DrawingClusterClass DrawingClusterClass;
typedef struct _DrawingShapeClass   DrawingShapeClass;
typedef enum   _DrawingShapeType    DrawingShapeType;

enum _DrawingShapeType
{
	DRAWING_SHAPE_TYPE_NULL,
	DRAWING_SHAPE_TYPE_CIRCLE,
	DRAWING_SHAPE_TYPE_CLUSTER,
	DRAWING_SHAPE_TYPE_DOCUMENT,
	DRAWING_SHAPE_TYPE_ELLIPSE,
	DRAWING_SHAPE_TYPE_LINE,
	DRAWING_SHAPE_TYPE_PATH,
	DRAWING_SHAPE_TYPE_RECTANGLE,
};

struct _DrawingShapeClass
{
	GObjectClass parent_class;
};

struct _DrawingClusterClass
{
	DrawingShapeClass parent_class;
};

G_DECLARE_DERIVABLE_TYPE (DrawingShape,             drawing_shape,              DRAWING, SHAPE,              GObject);
G_DECLARE_DERIVABLE_TYPE (DrawingCluster,           drawing_cluster,            DRAWING, CLUSTER,            DrawingShape);
G_DECLARE_FINAL_TYPE     (DrawingApplication,       drawing_application,        DRAWING, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE     (DrawingApplicationWindow, drawing_application_window, DRAWING, APPLICATION_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE     (DrawingCircle,            drawing_circle,             DRAWING, CIRCLE,             DrawingShape);
G_DECLARE_FINAL_TYPE     (DrawingDocument,          drawing_document,           DRAWING, DOCUMENT,           DrawingCluster);
G_DECLARE_FINAL_TYPE     (DrawingEllipse,           drawing_ellipse,            DRAWING, ELLIPSE,            DrawingShape);
G_DECLARE_FINAL_TYPE     (DrawingRectangle,         drawing_rectangle,          DRAWING, RECTANGLE,          DrawingShape);

/* Drawing */
GResource *drawing_get_resource      (void);
int        drawing_get_resource_path (char *buffer, size_t maxlen, const char *name);
GSettings *drawing_get_settings      (void);

/* Drawing Application */
GApplication *drawing_application_new (const char *application_id, GApplicationFlags flags);

/* Drawing Application Window */
GtkWidget *drawing_application_window_new (GApplication *application);
