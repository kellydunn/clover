// This class describes a utility that processes and samples of JACK audio

#include "clover.h"
#include "visualizer.h"
#include "audio-sampler.h"
#include <jack/jack.h>
#include <fftw3.h>

jack_default_audio_sample_t * AudioSampler::get_audio_sample_from_port(jack_port_t * port, int nframes) {
  return (jack_default_audio_sample_t *)jack_port_get_buffer(port, nframes);
}

int AudioSampler::process(jack_nframes_t nframes, void *args){
  Clover * clover = (Clover*) args;
  AudioSampler * ref = clover->get_audio_sampler();
  AudioSampler data = *ref;

  jack_default_audio_sample_t *input_r = ref->get_audio_sample_from_port(data.input_port_r, nframes);
  jack_default_audio_sample_t *input_l = ref->get_audio_sample_from_port(data.input_port_l, nframes);
  jack_default_audio_sample_t *output_r = ref->get_audio_sample_from_port(data.output_port_r, nframes);
  jack_default_audio_sample_t *output_l = ref->get_audio_sample_from_port(data.output_port_l, nframes);

  int i;
  // Spec: apply a window function to N samples to data
  for (i = 0; i < nframes; i++) {
    data.fftw_in[i] = data.window(((input_l[i] + input_r[i])/2), i);
  }


  //int val = ((intptr_t)(data.fftw_out[512]) * 5000) % 200;


  //if(val > 0.0) {
    //g_object_set(global_gst->vert, "speed", val, NULL);
    //Visualizer * gst = clover->get_visualizer();
    //g_object_set(gst->sol, "threshold", (int)val,NULL);
  // }

  // The first bin in the FFT is DC (0 Hz), the second bin is Fs / N, where Fs is the sample rate and N is the size of the FFT. 
  // The next bin is 2 * Fs / N. To express this in general terms, the nth bin is n * Fs / N.

  /*
  for(i=0;i<512;i++) {
    int index = i * 44100 / 1024;
    if (data.fftw_in[index] > 0.0) {
      printf("%f,  ", data.fftw_in[index]);
    }
  }
  printf("\n");
  */

  return 0;
}

double AudioSampler::window(jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos((2*PI*n)/((intptr_t)this->frames))) * (double)in;
}

jack_port_t * AudioSampler::register_port_by_name(char * name) {
  return jack_port_register(this->client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
}

// TODO it seems kinda nasty that the audio sampler needs to know about the "session"?
AudioSampler::AudioSampler(Clover * clover) {
  options = JackNoStartServer;
  client_name = "clover";
  server_name = NULL;
  this->clover = clover;

  this->client = jack_client_open(client_name, options, &status, server_name);

  if(this->client == NULL) {
    fprintf(stderr, "Could not open a connection to the JACK server.  Is JACK running?\n");
  }

  this->input_port_l = this->register_port_by_name((char *) "group_mix_in:l");
  this->input_port_r = this->register_port_by_name((char *) "group_mix_in:r");
  this->output_port_l = this->register_port_by_name((char *) "master_out:l");
  this->output_port_r = this->register_port_by_name((char *) "master_out:r");

  this->frames = (jack_nframes_t *) jack_get_buffer_size(this->client);
  this->fftw_in = (double *)fftw_malloc((intptr_t) frames * sizeof(double));
  this->fftw_out = (fftw_complex *)fftw_malloc((intptr_t) frames * sizeof(double));

  jack_set_process_callback(this->client, AudioSampler::process, (void*)this->clover);
  jack_activate(this->client);

  ports = jack_get_ports(this->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);

  if(ports != NULL) {
    printf("Attempting to connect ports...\n");
    jack_connect(this->client, jack_port_name(this->output_port_l), ports[0]);
    jack_connect(this->client, jack_port_name(this->output_port_r), ports[0]);
  }  
}