# include "clover-gst.h"

clover_gst_t * clover_gst_init(clover_gst_t * gst) {
  // Processing elements
  gst = (clover_gst_t *)malloc(sizeof(clover_gst_t));

  // Initialize general framework for the pipeline
  // TODO Abstract this out into seperate function
  gst->source = gst_element_factory_make("filesrc", "source");
  gst->decoder = gst_element_factory_make("decodebin2", "uridecoder");
  gst->sink = gst_element_factory_make("autovideosink", "autodetect");

  if(!gst->source | !gst->decoder | !gst->sink){
    printf("There was an error creating the processing elements.\n");
  }

  // Effect Elements
  // TODO Abstract this out into seperate function
  gst->ffmpegcolor = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace");
  gst->ffmpegcolor2 = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace2");
  gst->vert = gst_element_factory_make("vertigotv", "effectv");
  gst->sol = gst_element_factory_make("solarize", "gaudieffects");

  if(!gst->ffmpegcolor | !gst->ffmpegcolor2 | !gst->vert | !gst->sol){
    printf("There was an error creating the effect elements.\n");
  }

  // Pipeline and bins
  gst->pipeline = gst_pipeline_new("clover-pipeline");
  gst->processing_bin = gst_bin_new("clover-processing-bin");

  if(!gst->pipeline | !gst->processing_bin){
    printf("There was an error creating the processing bin.\n");
  }

  // Processing Bin
  // TODO Add effects based on configuration file
  gst_bin_add_many(GST_BIN(gst->processing_bin),
                   gst->decoder, gst->ffmpegcolor, gst->sol, gst->ffmpegcolor2, gst->sink, NULL);

  gst_element_link_many(gst->ffmpegcolor, gst->sol, gst->ffmpegcolor2, gst->sink, NULL);

  gst_element_add_pad(gst->processing_bin,
                      gst_ghost_pad_new("bin_sink",
                                        gst_element_get_static_pad(gst->decoder, "sink")));

  g_signal_connect(gst->decoder, "pad-added", G_CALLBACK(gst_pad_added), gst);

  // Link source video to processing utils
  gst_bin_add_many(GST_BIN(gst->pipeline), gst->source, gst->processing_bin, NULL);
  if(gst_element_link(gst->source, gst->processing_bin) != TRUE){
    printf("Could not link data-source to processing-bin");
  }

  return gst;
}

// The Gstreamer callback function
// Fairly standard, this callback simply forwards the media through the pipeline.
// TODO Look up doxygen style form of documentation 
static void gst_pad_added(GstElement *src, GstPad *new_pad, clover_gst_t *gst) {
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

