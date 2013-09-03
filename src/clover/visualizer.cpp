#include <gst/gst.h>
#include <gtk/gtk.h>
#include "visualizer.h"
#include <glib-object.h>

Visualizer::Visualizer() {
  // Initialize general framework for the pipeline
  // TODO Abstract this out into seperate function
  //source = gst_element_factory_make("filesrc", "source");
  this->set_source((char *) "filesrc", (char *) "source");
  decoder = gst_element_factory_make("decodebin2", "uridecoder");
  sink = gst_element_factory_make("autovideosink", "autodetect");

  if(!this->source | !decoder | !sink){
    printf("There was an error creating the processing elements.\n");
  }

  // Effect Elements
  // TODO Abstract this out into seperate function
  ffmpegcolor = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace");
  ffmpegcolor2 = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace2");
  vert = gst_element_factory_make("vertigotv", "effectv");
  sol = gst_element_factory_make("solarize", "gaudieffects");

  if(!ffmpegcolor | !ffmpegcolor2 | !vert | !sol){
    printf("There was an error creating the effect elements.\n");
  }

  // Pipeline and bins
  pipeline = gst_pipeline_new("clover-pipeline");
  processing_bin = gst_bin_new("clover-processing-bin");

  if(!pipeline | !processing_bin){
    printf("There was an error creating the processing bin.\n");
  }

  // Processing Bin
  // TODO Add effects based on configuration file one by one, and then  null terminate it.
  gst_bin_add_many(GST_BIN(processing_bin), decoder, ffmpegcolor, sol, ffmpegcolor2, sink, NULL);


  // TODO link elements based on configuration file one by one, and then null teriminated it.
  // NOTE visual effects appear to need their own ffmpegcolor start pad and sink pad.  
  //      Investigate linking all visual effects first, then linking to sink.
  gst_element_link_many(ffmpegcolor, sol, ffmpegcolor2, sink, NULL);

  gst_element_add_pad(processing_bin, gst_ghost_pad_new("bin_sink", gst_element_get_static_pad(decoder, "sink")));

  g_signal_connect(decoder, "pad-added", G_CALLBACK(Visualizer::pad_added), (gpointer)this);

  // Link source video to processing utils
  gst_bin_add_many(GST_BIN(pipeline), this->source, processing_bin, NULL);
  if(gst_element_link(this->source, processing_bin) != TRUE){
    printf("Could not link data-source to processing-bin");
  }
}

void Visualizer::set_source(char * type, char * name) {
  this->source = gst_element_factory_make(type, name);
}

GstElement * Visualizer::get_source() {
  return this->source;
}

// @pre The passed in {GstPad} has yet to be processed by the passed in {GstreamerClient} pipeline.
// @post The passed in {GstPad} has been processed by the passed in {GstreamerClient} pipeline.
// @param {GstElement} src.  A pointer to the source of the new {GstPad}.  
//                           For clover's purposes, this should only be a video source.
// @param {GstPad} new_pad.  A pointer to the pad that was generated during processing the demuxed video.
// @param {GstreamerClient} gst.  A pointer to the instance of the GstreamerClient that is processing video.
void Visualizer::pad_added(GstElement *src, GstPad *new_pad, Visualizer * gst) {
  GstPad *sink_pad = gst_element_get_static_pad(gst->ffmpegcolor, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;

  g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));
  if(gst_pad_is_linked(sink_pad)) {
    g_print("We are aldready linked.  Ignoring\n");
    goto exit;
  }

  new_pad_caps = gst_pad_get_caps(new_pad);
  new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
  new_pad_type = gst_structure_get_name(new_pad_struct);
  if(!g_str_has_prefix(new_pad_type, "video/x-raw")) {
    g_print("  It has type '%s' which is not raw video.   Ignoring.\n", new_pad_type);
    goto exit;
  }

  ret = gst_pad_link(new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_print ("  Type is '%s' but link failed.\n", new_pad_type);
  } else {
    g_print ("  Link succeeded (type '%s').\n", new_pad_type);
  }

exit:
  if(new_pad_caps != NULL) {
    gst_caps_unref(new_pad_caps);
  }

  gst_object_unref(sink_pad);
}

