// The main executable for clover
//   - initializes JACK client and Gstreamer pipeline
//   - play video file

#include "jack-client.h"
#include "visualizer.h"
#include "clover.h"

Clover::Clover(int * argc, char *** argv) {
  gst_init(argc, argv);

  Visualizer vis = Visualizer();
  this->visualizer = &vis;

  JackClient jack = JackClient();
  this->jack_client = &jack;  

  jack.set_clover(this);
}

void Clover::set_visualizer_source(char * src) {
  printf("Setting source to %s\n", src);
  g_object_set(this->visualizer->get_source(), "location", src, NULL);
}

Visualizer * Clover::get_visualizer() {
  return this->visualizer;
}

JackClient * Clover::get_jack_client() {
  return this->jack_client;
}

void Clover::play() { 
  // TODO implement 
}
