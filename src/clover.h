#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>

typedef struct _clover_gst_t {
  GstElement *pipeline;
  GstElement *source;
  GstElement *decoder;
  GstElement *sink;
  GstElement *processing_bin;
  GstElement *post_process_bin;
  GstElement *ffmpegcolor;
  GstElement *ffmpegcolor2;
  GstElement *vert;
  GstElement *sol;
  clover_jack_t *clover_jack_data;
} clover_gst_t;

static void gst_pad_added(GstElement *src, GstPad *new_pad, clover_gst_t *data);
