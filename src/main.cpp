// This is the main entry point for clover.
// Initializes all intresting classes, and begins accepting audio to guide visual effects.

#include <stdlib.h>
#include <gst/gst.h>
#include "clover/clover.h"
#include "clover/audio-sampler.h"
#include "clover/visualizer.h"

int main(int argc, char **argv) {
  if(argv[1] == NULL) {
    printf("No input video file detected.  Please pass in a video file.\n");
    return -1;
  }

  Clover clover = Clover(&argc, &argv);
  Clover * ref = &clover;
  clover.set_visualizer_source((char *) argv[1]);

  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  AudioSampler * jack = clover.get_audio_sampler();
  Visualizer * gst_ref = clover.get_visualizer();

  // TODO Parse command line arguments and throw accordingly
  gst_element_set_state(gst_ref->pipeline, GST_STATE_PLAYING);
  bus = gst_element_get_bus (gst_ref->pipeline);

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

        if (GST_MESSAGE_SRC (msg) == GST_OBJECT (gst_ref->pipeline)) {
          GstState old_state, new_state, pending_state;
          gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

          g_print ("Pipeline state changed from %s to %s:\n", 
                   gst_element_state_get_name (old_state), 
                   gst_element_state_get_name (new_state));

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
  gst_element_set_state (gst_ref->pipeline, GST_STATE_NULL);
  gst_object_unref (gst_ref->pipeline);

  return 0;
}
