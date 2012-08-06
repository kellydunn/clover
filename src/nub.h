#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>

typedef struct _NubData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *decoder;
  GstElement *sink;
  GstElement *processing_bin;
} NubData;

static void gst_pad_added(GstElement *src, GstPad *new_pad, NubData *data);
