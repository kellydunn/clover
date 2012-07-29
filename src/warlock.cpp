#include <stdio.h>
#include <jack/jack.h>
#include <sndfile.h>
#include <freej.h>

jack_client_t *client;
jack_options_t options = JackNoStartServer;
jack_status_t status;
jack_port_t *input_port;
jack_port_t *output_port;

const char *client_name = "warlock";
const char *server_name = NULL;

int process(jack_nframes_t frames, void *args){
  jack_default_audio_sample_t *output;
  output = (jack_default_audio_sample_t *)jack_port_get_buffer(output_port, frames);
  printf("%d\n", sizeof(output));
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

  jack_set_process_callback(client, process, NULL);
  for(;;){}
  return 0;
}
