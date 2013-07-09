#ifndef CLOVER_JACK_H
#define CLOVER_JACK_H

#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>
#include "gstreamer-client.h"

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
    void set_gstreamer_client(GstreamerClient * clover_gst);
    GstreamerClient * get_gstreamer_client();
    double window(jack_default_audio_sample_t in, int n);

  private:
    GstreamerClient * clover_gst;
}; 

const float PI = 3.14159254;

#endif