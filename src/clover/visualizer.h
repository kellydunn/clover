// This describes the fields and functionality of a GstreamerClient.

#ifndef _VISUALIZER_H
#define _VISUALIZER_H

#include <gst/gst.h>
#include <glib.h>

class Visualizer {
  public:
    GstElement *pipeline;
    //GstElement *source;
    GstElement *decoder;
    GstElement *sink;
    GstElement *processing_bin;
    GstElement *post_process_bin;
    GstElement *ffmpegcolor;
    GstElement *ffmpegcolor2;
    GstElement *vert;
    GstElement *sol;

    Visualizer();
    void set_source_element();
    GstElement * get_source_element();
    static void pad_added(GstElement *, GstPad *, Visualizer *);

  private:
    GstElement *source;
};

#endif
