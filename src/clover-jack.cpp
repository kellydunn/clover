#include "clover-gst.h"
#include "clover-jack.h"

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
    g_object_set(data->clover_gst->sol, "threshold", (int)val,NULL);
  }
  return 0;
}

clover_jack_t * clover_jack_init(clover_jack_t * jack) {
  jack = (clover_jack_t*)malloc(sizeof(clover_jack_t));

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

  return jack;
}
