/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#define VIEWER_RESOURCE_CCH 64
#define VIEWER_TYPE_APPLICATION        (viewer_application_get_type        ())
#define VIEWER_TYPE_APPLICATION_WINDOW (viewer_application_window_get_type ())

G_DECLARE_FINAL_TYPE (ViewerApplication,       viewer_application,        VIEWER, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerApplicationWindow, viewer_application_window, VIEWER, APPLICATION_WINDOW, GtkApplicationWindow);

/* Viewer */
GResource  *viewer_get_resource      (void);
int         viewer_get_resource_path (char *buffer, size_t maxlen, const char *name);

/* Viewer Application Window */
GtkWidget *viewer_application_window_new                (GApplication *application);
