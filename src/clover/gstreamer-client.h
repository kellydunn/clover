#ifndef CLOVER_GST_H
#define CLOVER_GST_H
#include <gst/gst.h>
#include <glib.h>

/*
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
} clover_gst_t;
*/

class GstreamerClient {
  public:
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

    GstreamerClient();
    static void pad_added(GstElement *, GstPad *, GstreamerClient *);
};

#endif
