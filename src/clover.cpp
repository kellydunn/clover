#include "clover-jack.h"
#include "clover.h"

GstBus * bus;
GstElement *sink_bin;
jack_port_t * input_port_l;
jack_port_t * input_port_r;
clover_gst_t *global_gst;

void clover_jack_init(clover_jack_t * jack);
void clover_gst_init(clover_gst_t * gst) {
  // Processing elements
  gst = (clover_gst_t *)malloc(sizeof(clover_gst_t));
  printf("Pre-playing\n");
  gst->source = gst_element_factory_make("filesrc", "source");
  gst->decoder = gst_element_factory_make("decodebin2", "uridecoder");
  gst->sink = gst_element_factory_make("autovideosink", "autodetect");
  printf("Created processing elements!\n");

  // Effect Elements
  gst->ffmpegcolor = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace");
  gst->ffmpegcolor2 = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace2");
  gst->vert = gst_element_factory_make("vertigotv", "effectv");
  gst->sol = gst_element_factory_make("solarize", "gaudieffects");
  printf("Created effect elements!\n");

  // Pipeline and bins
  gst->pipeline = gst_pipeline_new("clover-pipeline");
  gst->processing_bin = gst_bin_new("clover-processing-bin");
  printf("Created pipeline!\n");

  // Processing Bin
  gst_bin_add_many(GST_BIN(gst->processing_bin), gst->decoder,
                   gst->ffmpegcolor, gst->sol, gst->ffmpegcolor2, gst->sink, NULL);

  gst_element_link_many(gst->ffmpegcolor, gst->sol, gst->ffmpegcolor2, gst->sink, NULL);

  gst_element_add_pad(gst->processing_bin,
                      gst_ghost_pad_new("bin_sink",
                                        gst_element_get_static_pad(gst->decoder, "sink")));

  g_signal_connect(gst->decoder, "pad-added", G_CALLBACK(gst_pad_added), gst);
  printf("Attached processing events!\n");

  // Link source video to processing utils
  gst_bin_add_many(GST_BIN(gst->pipeline), gst->source, gst->processing_bin, NULL);
  if(gst_element_link(gst->source, gst->processing_bin) != TRUE){
    printf("Could not link data-source to processing-bin");
  }

  g_object_set(gst->source, "location", "/home/kelly/Videos/shogun-assassin.avi", NULL);
  printf("Linked source!\n");
}

int main(int argc, char **argv) {
  gst_init(&argc, &argv);

  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  //clover_jack_t * jack;
  //clover_jack_init(jack);

  clover_gst_t * gst;

  clover_gst_init(gst);
  global_gst = gst;

  gst_element_set_state(gst->pipeline, GST_STATE_PLAYING);
  bus = gst_element_get_bus (gst->pipeline);

  /*
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
  */
  return 0;
}

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

double window(clover_jack_t * jack, jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos(2*PI*n/(int)jack->frames)) * (double)in;
}

int process(jack_nframes_t nframes, void *args){
  clover_jack_t *data = (clover_jack_t*)args;

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
    //g_object_set(global_gst->vert, "speed", val, NULL);
    g_object_set(global_gst->sol, "threshold", (int)val,NULL);
  }
  return 0;
}

void clover_jack_init(clover_jack_t * jack) {
  /*
  jack->options = JackNoStartServer;
  jack->client_name = "clover";
  jack->server_name = NULL;

  jack->client = jack_client_open(jack->client_name, jack->options, &jack->status, jack->server_name);
  if(jack->client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?\n");
  }

  jack_set_process_callback(jack->client, process, (void*)jack);
  jack_activate(jack->client);

  jack->frames = (jack_nframes_t *)jack_get_buffer_size(jack->client);
  jack->fftw_in = (double *)fftw_malloc((int)jack->frames * sizeof(double));
  jack->fftw_out = (fftw_complex *)fftw_malloc((int)jack->frames * sizeof(double));

  jack->input_port_l = jack_port_register(jack->client, "group_mix_in:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  jack->input_port_r = jack_port_register(jack->client, "group_mix_in:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  jack->output_port_l = jack_port_register(jack->client, "master_out:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  jack->output_port_r = jack_port_register(jack->client, "master_out:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  jack->ports = jack_get_ports(jack->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(jack->ports != NULL) {
    jack_connect(jack->client, jack_port_name(jack->output_port_l), jack->ports[0]);
    jack_connect(jack->client, jack_port_name(jack->output_port_r), jack->ports[0]);
  }
  */
}
