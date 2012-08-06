#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>

typedef struct _NubData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *decoder;
  GstElement *sink;
  GstElement *processing_bin;
  GstElement *post_process_bin;
  GstElement *ffmpegcolor;
  GstElement *vert;
  GstElement *sol;
  NubJackData *nub_jack_data;
} NubData;

typedef struct _NubParams {
  NubData * nub_data;
  NubJackData * nub_jack_data;
} NubParams;

static void gst_pad_added(GstElement *src, GstPad *new_pad, NubData *data);
