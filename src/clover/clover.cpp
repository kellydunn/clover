// The main executable for clover
//   - initializes JACK client and Gstreamer pipeline
//   - play video file

#include "audio-sampler.h"
#include "visualizer.h"
#include "clover.h"

Clover::Clover(int * argc, char *** argv) {
  gst_init(argc, argv);

  this->visualizer = new Visualizer();
  this->audio_sampler = new AudioSampler();
}

void Clover::set_visualizer_source(char * src) {
  printf("Setting source to %s\n", src);
  g_object_set(this->visualizer->get_source(), "location", src, NULL);
}

Visualizer * Clover::get_visualizer() {
  return this->visualizer;
}

AudioSampler * Clover::get_audio_sampler() {
  return this->audio_sampler;
}

void Clover::play() { 
  // TODO implement 
}

