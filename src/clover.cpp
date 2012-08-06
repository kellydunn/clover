#include "clover-jack.h"
#include "clover.h"

GstBus * bus;
GstElement *ffmpegcolor2;
GstElement *sink_bin;
static GMainLoop *loop;

CloverJackData clover_jack_init();
jack_port_t * input_port_l;
jack_port_t * input_port_r;
CloverData *global_data;

int main(int argc, char **argv) {
  CloverData data;
  global_data = &data;
  CloverJackData jack_data = clover_jack_init();
  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  gst_init(&argc, &argv);
  data.clover_jack_data = &jack_data;

  // Processing elements
  data.source = gst_element_factory_make("filesrc", "source");
  data.decoder = gst_element_factory_make("decodebin2", "uridecoder");
  data.sink = gst_element_factory_make("autovideosink", "autodetect");

  // Effect Elements
  data.ffmpegcolor = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace");
  ffmpegcolor2 = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace2");
  data.vert = gst_element_factory_make("vertigotv", "effectv");
  data.sol = gst_element_factory_make("solarize", "gaudieffects");

  // Pipeline and bins
  data.pipeline = gst_pipeline_new("clover-pipeline");
  data.processing_bin = gst_bin_new("clover-processing-bin");

  // Processing Bin
  gst_bin_add_many(GST_BIN(data.processing_bin), data.decoder,
                   data.ffmpegcolor, data.sol, ffmpegcolor2, data.sink, NULL);
  gst_element_link_many(data.ffmpegcolor, data.sol, ffmpegcolor2, data.sink, NULL);

  gst_element_add_pad(data.processing_bin,
                      gst_ghost_pad_new("bin_sink",
                                        gst_element_get_static_pad(data.decoder, "sink")));

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

static void gst_pad_added(GstElement *src, GstPad *new_pad, CloverData *data) {
  GstPad *sink_pad = gst_element_get_static_pad(data->ffmpegcolor, "sink");
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

double window(CloverJackData * data, jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos(2*PI*n/(int)data->frames)) * (double)in;
}

int process(jack_nframes_t nframes, void *args){
  CloverJackData *data = (CloverJackData*)args;

  jack_default_audio_sample_t *input_r;
  jack_default_audio_sample_t *input_l;
  int i;

  input_r = (jack_default_audio_sample_t *)jack_port_get_buffer(data->input_port_r, nframes);
  input_l = (jack_default_audio_sample_t *)jack_port_get_buffer(data->input_port_l, nframes);
  (jack_default_audio_sample_t *)jack_port_get_buffer(data->output_port_r, nframes);
  (jack_default_audio_sample_t *)jack_port_get_buffer(data->output_port_l, nframes);
  for (i = 0; i < nframes; i++) {
    data->fftw_in[i] = window(data, ((input_l[i] + input_r[i])/2), i);
  }

  int val = (((int)(data->fftw_in[512]*5000)) % 200);

  if(val > 0.0) {
    //g_object_set(global_data->vert, "speed", val, NULL);
    g_object_set(global_data->sol, "threshold", (int)val,NULL);
  }
  return 0;
}

CloverJackData clover_jack_init() {
  CloverJackData data;

  data.options = JackNoStartServer;
  data.client_name = "clover";
  data.server_name = NULL;

  data.client = jack_client_open(data.client_name, data.options, &data.status, data.server_name);
  if(data.client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?\n");
  }

  jack_set_process_callback(data.client, process, (void*)&data);
  jack_activate(data.client);

  data.frames = (jack_nframes_t *)jack_get_buffer_size(data.client);
  data.fftw_in = (double *)fftw_malloc((int)data.frames * sizeof(double));
  data.fftw_out = (fftw_complex *)fftw_malloc((int)data.frames * sizeof(double));

  data.input_port_l = jack_port_register(data.client, "group_mix_in:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  data.input_port_r = jack_port_register(data.client, "group_mix_in:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  data.output_port_l = jack_port_register(data.client, "master_out:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  data.output_port_r = jack_port_register(data.client, "master_out:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  data.ports = jack_get_ports(data.client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(data.ports != NULL) {
    jack_connect(data.client, jack_port_name(data.output_port_l), data.ports[0]);
    jack_connect(data.client, jack_port_name(data.output_port_r), data.ports[0]);
  }

  return data;
}
