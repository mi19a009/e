/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#define APPLICATION_ID    "com.github.mi19a009.PictureViewer"
#define APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define GETTEXT_CODESET   "UTF-8"
#define GETTEXT_PACKAGE   "github-mi19a009-e"
#define GSETTINGS_KEY     "GSETTINGS_SCHEMA_DIR"
#define GSETTINGS_VALUE   "."
#define LOCALE            ""
#define RESOURCE_FORMAT "/com/github/mi19a009/PictureViewer/%s"
#ifndef GETTEXT_PATH
#define GETTEXT_PATH "locale"
#endif

extern GApplication *viewer_application_new (const char *application_id, GApplicationFlags flags);

/*******************************************************************************
アプリケーションのメイン エントリ ポイントです。
*/
int
main (int argc, char *argv [])
{
	GApplication *application;
	int exitcode;
#ifndef NDEBUG
	g_setenv (GSETTINGS_KEY, GSETTINGS_VALUE, FALSE);
#endif
	setlocale (LC_ALL, LOCALE);
	bindtextdomain (GETTEXT_PACKAGE, GETTEXT_PATH);
	bind_textdomain_codeset (GETTEXT_PACKAGE, GETTEXT_CODESET);
	textdomain (GETTEXT_PACKAGE);
	application = viewer_application_new (APPLICATION_ID, APPLICATION_FLAGS);
	exitcode = g_application_run (application, argc, argv);
	g_object_unref (application);
	return exitcode;
}

/*******************************************************************************
リソースへのパスを取得します。
*/
int
viewer_get_resource_path (char *buffer, size_t maxlen, const char *name)
{
	return g_snprintf (buffer, maxlen, RESOURCE_FORMAT, name);
}
