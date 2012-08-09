#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>

typedef struct _clover_jack_t {
  jack_client_t *client;
  jack_options_t options;
  jack_status_t status;
  jack_port_t *input_port_r;
  jack_port_t *input_port_l;
  jack_port_t *output_port_r;
  jack_port_t *output_port_l;
  jack_nframes_t *frames;
  double *fftw_in;
  fftw_complex *fftw_out;
  const char *client_name;
  const char *server_name;
  const char **ports;
} clover_jack_t;

const int PI = 3.14159254;

double window(jack_default_audio_sample_t in, int n);
int process(jack_nframes_t nframes, void *args);
clover_jack_t * clover_jack_init(clover_jack_t * jack);
