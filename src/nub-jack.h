#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>

typedef struct _NubJackData {
  jack_client_t *client,
  jack_options_t options = JackNoStartServer,
  jack_status_t status,
  jack_port_t *input_port_r,
  jack_port_t *input_port_l,
  jack_port_t *output_port_r,
  jack_port_t *output_port_l,
  jack_nframes_t *frames,
  const char *client_name = "nub",
  const char *server_name = NULL,
  const char **ports
} NubJackData;

const int PI = 3.14159254;

double window(jack_default_audio_sample_t in, int n);
int process(jack_nframes_t nframes, void *args);
