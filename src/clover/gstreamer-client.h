// This describes the fields and functionality of a GstreamerClient.

#ifndef CLOVER_GST_H
#define CLOVER_GST_H

#include <gst/gst.h>
#include <glib.h>

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
