/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "drawing.h"
#define APPLICATION_ID    "com.github.mi19a009.Draw"
#define APPLICATION_FLAGS G_APPLICATION_HANDLES_OPEN
#define LOCALE            ""
#define RESOURCE_FORMAT   "/com/github/mi19a009/Draw/%s"

/*******************************************************************************
アプリケーションのメイン エントリ ポイントです。
*/
int
main (int argc, char *argv [])
{
	GApplication *application;
	int exitcode;
	setlocale (LC_ALL, LOCALE);
	bindtextdomain (GETTEXT_PACKAGE, GETTEXT_PATH);
	bind_textdomain_codeset (GETTEXT_PACKAGE, GETTEXT_CODESET);
	textdomain (GETTEXT_PACKAGE);
	application = drawing_application_new (APPLICATION_ID, APPLICATION_FLAGS);
	exitcode = g_application_run (application, argc, argv);
	g_object_unref (application);
	return exitcode;
}

/*******************************************************************************
リソースへのパスを取得します。
*/
int
drawing_get_resource_path (char *buffer, size_t maxlen, const char *name)
{
	return g_snprintf (buffer, maxlen, RESOURCE_FORMAT, name);
}

/*******************************************************************************
アプリケーションの環境設定を取得します。
*/
GSettings *
drawing_get_settings (void)
{
	return g_settings_new (APPLICATION_ID);
}
