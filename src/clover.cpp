#include "clover-jack.h"
#include "clover-gst.h"
#include "clover.h"

clover_gst_t * clover_gst_init(clover_gst_t * gst) {
  // Processing elements
   gst = (clover_gst_t *)malloc(sizeof(clover_gst_t));

  gst->source = gst_element_factory_make("filesrc", "source");
  gst->decoder = gst_element_factory_make("decodebin2", "uridecoder");
  gst->sink = gst_element_factory_make("autovideosink", "autodetect");

  if(!gst->source | !gst->decoder | !gst->sink){
    printf("There was an error creating the processing elements.\n");
  }

  // Effect Elements
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

int main(int argc, char **argv) {
  gst_init(&argc, &argv);

  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  clover_jack_t * jack;
  jack = clover_jack_init(jack);

  clover_gst_t * gst;
  gst = clover_gst_init(gst);

  jack->clover_gst = gst;

  if(argv[1] == NULL) {
    printf("No input video file detected.  Please pass in a video file.\n");
    return -1;
  }

  g_object_set(gst->source, "location", argv[1], NULL);
  printf("Linked source!\n");

  gst_element_set_state(gst->pipeline, GST_STATE_PLAYING);
  bus = gst_element_get_bus (gst->pipeline);

  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR);

    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        terminate = TRUE;
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        terminate = TRUE;
        break;
      case GST_MESSAGE_STATE_CHANGED:

        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (gst->pipeline)) {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
          g_print ("Pipeline state changed from %s to %s:\n",
                   gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
        }
        break;
      default:

        g_printerr ("Unexpected message received.\n");
        break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);


  gst_object_unref (bus);
  gst_element_set_state (gst->pipeline, GST_STATE_NULL);
  gst_object_unref (gst->pipeline);

  return 0;
}

static void gst_pad_added(GstElement *src, GstPad *new_pad, clover_gst_t *gst) {
  printf("woah");
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

