#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>
#include <gst/gst.h>
#include <glib.h>

jack_client_t *client;
jack_options_t options = JackNoStartServer;
jack_status_t status;
jack_port_t *input_port_r;
jack_port_t *input_port_l;
jack_port_t *output_port_r;
jack_port_t *output_port_l;
jack_nframes_t *frames;

const char *client_name = "warlock";
const char *server_name = NULL;
const char **ports;

const int PI = 3.14159254;

double window(jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos(2*PI*n/(int)frames)) * (double)in;
}

int process(jack_nframes_t nframes, void *args){
  jack_default_audio_sample_t *input_r;
  jack_default_audio_sample_t *input_l;
  double *fftw_in = (double *)fftw_malloc((int)frames * sizeof(double));
  int i;

  input_r = (jack_default_audio_sample_t *)jack_port_get_buffer(input_port_r, nframes);
  input_l = (jack_default_audio_sample_t *)jack_port_get_buffer(input_port_l, nframes);
  for (i = 0; i < nframes; i++) {
    fftw_in[i] = window((input_l[i]+input_r[i]/2), i);
    if(fftw_in[i] > 0.0) {
      printf("%f", fftw_in[i]);
    }
  }

  return 0;
};

int main(int argc, char **argv) {
  // JACK
  client = jack_client_open(client_name, options, &status, server_name);
  if(client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?\n");
    return 1;
  }

  jack_set_process_callback(client, process, NULL);
  jack_activate(client);

  frames = (jack_nframes_t *)jack_get_buffer_size(client);

  input_port_l = jack_port_register(client, "group_mix_in:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  input_port_r = jack_port_register(client, "group_mix_in:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  output_port_l = jack_port_register(client, "master_out:l", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  output_port_r = jack_port_register(client, "master_out:r", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(ports != NULL) {
    jack_connect(client, jack_port_name(output_port_l), ports[0]);
    jack_connect(client, jack_port_name(output_port_r), ports[0]);
  }

  gst_init(&argc, &argv);
  GError *error = NULL;
  GstBus *bus;
  GstPad *pad, *ghost_pad;
  GstElement *pipeline, *bin, *vert;

  pipeline = gst_parse_launch("playbin2 uri=http://docs.gstreamer.com/media/sintel_trailer-480p.webm", NULL);

  vert = gst_element_factory_make("vertigotv", "effectv");

  bin = gst_bin_new("video_sink_bin");
  gst_bin_add(GST_BIN(bin), vert);
  gst_element_link(vert, bin);

  pad = gst_element_get_static_pad (vert, "sink");
  ghost_pad = gst_ghost_pad_new ("sink", pad);
  gst_pad_set_active (ghost_pad, TRUE);
  gst_element_add_pad (bin, ghost_pad);
  gst_object_unref (pad);

  g_object_set (G_OBJECT (vert), "speed", (gfloat)5, NULL);
  g_object_set (G_OBJECT (vert), "zoom-speed", (gfloat)1.05, NULL);

  g_object_set (GST_OBJECT (pipeline), "video-sink", bin, NULL);
  gst_element_set_state(pipeline, GST_STATE_PLAYING);


  for(;;){}
  return 0;
}
