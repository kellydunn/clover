#include "gstreamer-client.h"
#include "jack-client.h"
#include <fftw3.h>

double window(clover_jack_t * jack, jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos((2*PI*n)/((intptr_t)jack->frames))) * (double)in;  
}

jack_default_audio_sample_t * get_audio_sample_from_port(jack_port_t * port, int nframes) {
  return (jack_default_audio_sample_t *)jack_port_get_buffer(port, nframes);
}

int process(jack_nframes_t nframes, void *args){
  clover_jack_t *data = (clover_jack_t*)args;

  jack_default_audio_sample_t *input_r;
  jack_default_audio_sample_t *input_l;
  jack_default_audio_sample_t *output_r;
  jack_default_audio_sample_t *output_l;


  input_r = get_audio_sample_from_port(data->input_port_r, nframes);
  input_l = get_audio_sample_from_port(data->input_port_l, nframes);
  output_r = get_audio_sample_from_port(data->output_port_r, nframes);
  output_l = get_audio_sample_from_port(data->output_port_l, nframes);

  int i;
  // Spec: apply a window function to N samples to data
  for (i = 0; i < nframes; i++) {
    data->fftw_in[i] = window(data, ((input_l[i] + input_r[i])/2), i);
  }

  // Spec: 
  fftw_plan p;
  p = fftw_plan_dft_1d(nframes, (fftw_complex*) data->fftw_in, data->fftw_out, FFTW_FORWARD, FFTW_ESTIMATE);
  //fftw_execute(p);

  int val = ((intptr_t)(&data->fftw_in[512]) * 5000) % 200;

  if(val > 0.0) {
    //g_object_set(global_gst->vert, "speed", val, NULL);
    g_object_set(data->clover_gst->sol, "threshold", (int)val,NULL);
  }

  return 0;
}

jack_port_t * register_port_by_name(clover_jack_t * jack, char * name) {
  return jack_port_register(jack->client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
}

jack_port_t * JackClient::register_port_by_name(char * name) {
  return jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
}

//void JackClient::set_gstreamer_client(clover_gst_t * clover_gst) {
void JackClient::set_gstreamer_client(GstreamerClient * clover_gst) {
  this->clover_gst = clover_gst;
}

JackClient::JackClient() {
  options = JackNoStartServer;
  client_name = "clover";
  server_name = NULL;

  client = jack_client_open(client_name, options, &status, server_name);

  if(client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?\n");
  }

  input_port_l = register_port_by_name((char *) "group_mix_in:l");
  input_port_r = register_port_by_name((char *) "group_mix_in:r");
  output_port_l = register_port_by_name((char *) "master_out:l");
  output_port_r = register_port_by_name((char *) "master_out:r");

  frames = (jack_nframes_t *) jack_get_buffer_size(client);
  fftw_in = (double *)fftw_malloc((intptr_t) frames * sizeof(double));
  fftw_out = (fftw_complex *)fftw_malloc((intptr_t) frames * sizeof(double));

  jack_set_process_callback(client, process, (void*)this);
  jack_activate(client);

  ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(ports != NULL) {
    jack_connect(client, jack_port_name(output_port_l), ports[0]);
    jack_connect(client, jack_port_name(output_port_r), ports[0]);
  }  
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

  jack->input_port_l = register_port_by_name(jack, (char *) "group_mix_in:l");
  jack->input_port_r = register_port_by_name(jack, (char *) "group_mix_in:r");
  jack->output_port_l = register_port_by_name(jack, (char *) "master_out:l");
  jack->output_port_r = register_port_by_name(jack, (char *) "master_out:r");

  jack->frames = (jack_nframes_t *) jack_get_buffer_size(jack->client);
  jack->fftw_in = (double *)fftw_malloc((intptr_t)jack->frames * sizeof(double));
  jack->fftw_out = (fftw_complex *)fftw_malloc((intptr_t)jack->frames * sizeof(double));

  jack_set_process_callback(jack->client, process, (void*)jack);
  jack_activate(jack->client);

  jack->ports = jack_get_ports(jack->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(jack->ports != NULL) {
    jack_connect(jack->client, jack_port_name(jack->output_port_l), jack->ports[0]);
    jack_connect(jack->client, jack_port_name(jack->output_port_r), jack->ports[0]);
  }
  return jack;
}
