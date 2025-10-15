/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE (ViewerApplication,       viewer_application,        VIEWER, APPLICATION,        GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerApplicationWindow, viewer_application_window, VIEWER, APPLICATION_WINDOW, GtkApplicationWindow);

#define VIEWER_TYPE_APPLICATION        (viewer_application_get_type        ())
#define VIEWER_TYPE_APPLICATION_WINDOW (viewer_application_window_get_type ())

/* Viewer */
GResource  *viewer_get_resource (void);

/* Viewer Application Window */
GtkWidget *viewer_application_window_new                (GApplication *application);
