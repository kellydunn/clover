#include <stdio.h>
#include <math.h>
#include <jack/jack.h>
#include <sndfile.h>
#include <freej.h>

jack_client_t *client;
jack_options_t options = JackNoStartServer;
jack_status_t status;
jack_port_t *input_port;
jack_port_t *output_port;
jack_nframes_t *frames;
const char *client_name = "warlock";
const char *server_name = NULL;
const char **ports;

const int PI = 3.14159254;

// Shamlessly ripped from the interwebs
//   von Hann window
double window(jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos(2*PI*n/(int)frames)) * (double)in;
}

int process(jack_nframes_t nframes, void *args){
  jack_default_audio_sample_t *output;
  double *fftw_in;
  int i;

  output = (jack_default_audio_sample_t *)jack_port_get_buffer(output_port, nframes);
  for (i = 0; i < nframes; i++) {
    fftw_in[i] = window(output[i], i);
  }

  return 0;
};

int main(int argc, char **argv) {
  client = jack_client_open(client_name, options, &status, server_name);
  if(client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?");
    return 1;
  }

  input_port = jack_port_register(client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  output_port = jack_port_register(client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
  if(ports != NULL) {
    jack_connect(client, jack_port_name(output_port), ports[0]);
  }

  jack_set_process_callback(client, process, NULL);
  jack_activate(client);
  for(;;){}
  return 0;
}
