// This describes the fields and functionality of a JackClient

#ifndef CLOVER_JACK_H
#define CLOVER_JACK_H

#include <stdio.h>
#include <math.h>
#include <fftw3.h>
#include <jack/jack.h>

class Clover;
class Visualizer;

#include "visualizer.h" // TODO this should not be needed
#include "clover.h"

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
    double window(jack_default_audio_sample_t in, int n);
    jack_default_audio_sample_t * get_audio_sample_from_port(jack_port_t * port, int nframes);
    static int process(jack_nframes_t nframes, void * args);
    void set_clover(Clover * clover);

  private:
    Clover * clover;
}; 

const float PI = 3.14159254;

#endif
