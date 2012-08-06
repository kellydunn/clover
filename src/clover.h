#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>

typedef struct _CloverData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *decoder;
  GstElement *sink;
  GstElement *processing_bin;
  GstElement *post_process_bin;
  GstElement *ffmpegcolor;
  GstElement *vert;
  GstElement *sol;
  CloverJackData *clover_jack_data;
} CloverData;

typedef struct _CloverParams {
  CloverData * clover_data;
  CloverJackData * clover_jack_data;
} CloverParams;

static void gst_pad_added(GstElement *src, GstPad *new_pad, CloverData *data);
