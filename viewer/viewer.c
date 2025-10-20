/* Copyright (C) 2025 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define PIXBUF_BITS_PER_SAMPLE 8
#define PIXBUF_OVERALL_ALPHA 255
#define PIXBUF_SCALE_X 1.0
#define PIXBUF_SCALE_Y 1.0

static void             viewer_composite               (GdkPixbuf **pixbuf, int width, int height);
static void             viewer_copy_pixels             (const guchar *source, guchar *destination, int width, int height, int stride);
static GdkPixbuf       *viewer_create_pixbuf_from_file (GFile *file, GError **error);
static cairo_surface_t *viewer_create_surface_for_data (cairo_t *cairo, guchar *data, int width, int height, int stride);

/*******************************************************************************
指定した画像の透過を有効にします。
*/
static void
viewer_composite (GdkPixbuf **pixbuf, int width, int height)
{
	GdkPixbuf *destination, *source;
	destination = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, PIXBUF_BITS_PER_SAMPLE, width, height);
	source = *pixbuf;
	*pixbuf = destination;
	gdk_pixbuf_composite (source, destination, 0, 0, width, height, 0, 0, PIXBUF_SCALE_X, PIXBUF_SCALE_Y, GDK_INTERP_NEAREST, PIXBUF_OVERALL_ALPHA);
	g_object_unref (source);
}

/*******************************************************************************
指定した画像のチャンネルを並び替えます。
*/
static void
viewer_copy_pixels (const guchar *source, guchar *destination, int width, int height, int stride)
{
	const guchar *src;
	guchar *dest;
	int x, y, offset;
	offset = 0;

	for (y = 0; y < height; y++)
	{
		src = source + offset;
		dest = destination + offset;

		for (x = 0; x < width; x++)
		{
			*(dest++) = src [2];
			*(dest++) = src [1];
			*(dest++) = src [0];
			*(dest++) = src [3];
			src += 4;
		}

		offset += stride;
	}
}

/*******************************************************************************
画像ファイルを開きます。
*/
static GdkPixbuf *
viewer_create_pixbuf_from_file (GFile *file, GError **error)
{
	GdkPixbuf *pixbuf;
	GFileInputStream *stream;
	stream = g_file_read (file, NULL, error);

	if (stream)
	{
		pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, error);
		g_object_unref (stream);
	}
	else
	{
		pixbuf = NULL;
	}

	return pixbuf;
}

/*******************************************************************************
画像ファイルを開きます。
*/
cairo_surface_t *
viewer_create_surface_from_file (cairo_t *cairo, GFile *file, GError **error)
{
	cairo_surface_t *surface;
	GdkPixbuf *pixbuf;
	guchar *data;
	int width, height, stride;
	pixbuf = viewer_create_pixbuf_from_file (file, error);

	if (pixbuf)
	{
		width = gdk_pixbuf_get_width (pixbuf);
		height = gdk_pixbuf_get_height (pixbuf);
		stride = gdk_pixbuf_get_rowstride (pixbuf);
		viewer_composite (&pixbuf, width, height);
		data = g_malloc (height * stride);
		viewer_copy_pixels (gdk_pixbuf_read_pixels (pixbuf), data, width, height, stride);
		g_object_unref (pixbuf);
		surface = viewer_create_surface_for_data (cairo, data, width, height, stride);
		g_free (data);
	}
	else
	{
		surface = NULL;
	}

	return surface;
}

/*******************************************************************************
画像を作成します。
*/
static cairo_surface_t *
viewer_create_surface_for_data (cairo_t *cairo, guchar *data, int width, int height, int stride)
{
	cairo_surface_t *surface, *image;
	image = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, stride);

	if (image)
	{
		surface = cairo_surface_create_similar (cairo_get_target (cairo), CAIRO_CONTENT_COLOR_ALPHA, width, height);

		if (surface)
		{
			cairo = cairo_create (surface);

			if (cairo)
			{
				cairo_set_antialias (cairo, CAIRO_ANTIALIAS_NONE);
				cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
				cairo_set_source_surface (cairo, image, 0, 0);
				cairo_paint (cairo);
				cairo_destroy (cairo);
			}
		}

		cairo_surface_destroy (image);
	}
	else
	{
		surface = NULL;
	}

	return surface;
}
