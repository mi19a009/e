/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define VIEWER_RESOURCE_PATH_CCH 64
#define VIEWER_TYPE_APPLICATION        (viewer_application_get_type        ())
#define VIEWER_TYPE_APPLICATION_WINDOW (viewer_application_window_get_type ())
#define PARAM_SPEC_BOOLEAN(PROPERTY) (g_param_spec_boolean ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB),                                                             (PROPERTY ## _DEFAULT_VALUE), (PROPERTY ## _FLAGS)))
#define PARAM_SPEC_FLOAT(PROPERTY)   (g_param_spec_float   ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _MINIMUM_VALUE), (PROPERTY ## _MAXIMUM_VALUE), (PROPERTY ## _DEFAULT_VALUE), (PROPERTY ## _FLAGS)))
#define PARAM_SPEC_OBJECT(PROPERTY)  (g_param_spec_object  ((PROPERTY ## _NAME), (PROPERTY ## _NICK), (PROPERTY ## _BLURB), (PROPERTY ## _OBJECT_TYPE),                                                               (PROPERTY ## _FLAGS)))

G_DECLARE_FINAL_TYPE (ViewerApplication,       viewer_application,        VIEWER, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerApplicationWindow, viewer_application_window, VIEWER, APPLICATION_WINDOW, GtkApplicationWindow);

/* Viewer */
cairo_surface_t *viewer_create_surface_from_file (cairo_t *cairo, GFile *file, int *surface_width, int *surface_height, GError **error);
GResource       *viewer_get_resource             (void);
int              viewer_get_resource_path        (char *buffer, size_t maxlen, const char *name);
GSettings       *viewer_get_settings             (void);

/* Viewer Application */
GApplication *viewer_application_new (const char *application_id, GApplicationFlags flags);

/* Viewer Application Window */
void       viewer_application_window_get_background (ViewerApplicationWindow *self, float *red, float *green, float *blue);
GFile     *viewer_application_window_get_file       (ViewerApplicationWindow *self);
float      viewer_application_window_get_zoom       (ViewerApplicationWindow *self);
GtkWidget *viewer_application_window_new            (GApplication *application);
void       viewer_application_window_set_background (ViewerApplicationWindow *self, float red, float green, float blue);
void       viewer_application_window_set_file       (ViewerApplicationWindow *self, GFile *file);
void       viewer_application_window_set_zoom       (ViewerApplicationWindow *self, float zoom);
