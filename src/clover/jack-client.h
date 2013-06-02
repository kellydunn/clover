#ifndef CLOVER_JACK_H
#define CLOVER_JACK_H
#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>
#include "gstreamer-client.h"

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
  clover_gst_t * clover_gst;
} clover_jack_t;

class JackClient {
  public:
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
    
    JackClient();
    jack_port_t * register_port_by_name(char *);
    void set_gstreamer_client(clover_gst_t * clover_gst);

  private:
    clover_gst_t * clover_gst;
}; 

// const int PI = 3.14159254;
const float PI = 3.14159254;

double window(jack_default_audio_sample_t in, int n);
int process(jack_nframes_t nframes, void *args);
clover_jack_t * clover_jack_init(clover_jack_t * jack);
jack_default_audio_sample_t * get_audio_sample_from_port(jack_port_t * port, int nframes);
jack_port_t * register_port_by_name(clover_jack_t * jack, char * name);
#endif
