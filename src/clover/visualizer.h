// This describes the fields and functionality of a GstreamerClient.

#ifndef _VISUALIZER_H
#define _VISUALIZER_H

#include <gst/gst.h>
#include <glib.h>

class Visualizer {
  public:
    GstElement *pipeline;
    GstElement *decoder;
    GstElement *sink;
    GstElement *processing_bin;
    GstElement *post_process_bin;
    GstElement *ffmpegcolor;
    GstElement *ffmpegcolor2;
    GstElement *vert;
    GstElement *sol;

    // Constructor
    Visualizer();

    // source for Gstreamer pipeline
    void set_source(char *, char *);
    GstElement * get_source();

    // decoder for Gstreamer pipeline
    void set_decoder(char *, char *);
    GstElement * get_decoder();

    static void pad_added(GstElement *, GstPad *, Visualizer *);

  private:
    GstElement *source;
};

#endif
