#include "nub-jack.h"


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
  NubJackData *data;
  // JACK
  /*
  data.client = jack_client_open(data.client_name, data.options, &data.status, data.server_name);
  if(data.client == NULL) {
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
  */

}
