#include "nub.h"

GstBus * bus;
GstElement *sol, *vert;
GstElement *ffmpegcolor;
GstElement *sink_bin;
static GMainLoop *loop;

int main(int argc, char **argv) {
  NubData data;
  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  gst_init(&argc, &argv);

  data.source = gst_element_factory_make("filesrc", "source");
  data.decoder = gst_element_factory_make("decodebin2", "uridecoder");
  data.sink = gst_element_factory_make("autovideosink", "autodetect");

  data.pipeline = gst_pipeline_new("nub-pipeline");
  data.processing_bin = gst_bin_new("nub-processing-bin");

  if (!data.pipeline || !data.source || !data.decoder || !data.sink || !data.processing_bin) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  gst_bin_add_many(GST_BIN(data.processing_bin), data.decoder, data.sink, NULL);
  gst_element_add_pad(data.processing_bin, gst_ghost_pad_new("bin_sink", gst_element_get_static_pad(data.decoder, "sink")));

  g_signal_connect(data.decoder, "pad-added", G_CALLBACK(gst_pad_added), &data);

  gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.processing_bin, NULL);
  if(gst_element_link(data.source, data.processing_bin) != TRUE){
    printf("Could not link data-source to processing-bin");
    return -1;
  }

  g_object_set(data.source, "location", "/home/kelly/Videos/shogun-assassin.avi", NULL);

  gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
  bus = gst_element_get_bus (data.pipeline);

  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR);

    /* Parse message */
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
        /* We are only interested in state-changed messages from the pipeline */
        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
          g_print ("Pipeline state changed from %s to %s:\n",
                   gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
        }
        break;
      default:
        /* We should not reach here */
        g_printerr ("Unexpected message received.\n");
        break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);

  return 0;
}

static void gst_pad_added(GstElement *src, GstPad *new_pad, NubData *data) {
  GstPad *sink_pad = gst_element_get_static_pad(data->sink, "sink");
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
