#ifndef _CLOVER_H
#define _CLOVER_H

#include <stdio.h>
#include <jack/jack.h>

class AudioSampler;
class Visualizer;

#include "visualizer.h"
#include "audio-sampler.h"

class Clover {
  public:
    Clover(int * argc, char ***argv);
    void set_visualizer_source(char *);
    void play();
    Visualizer * get_visualizer();
    AudioSampler * get_audio_sampler();

  private:
    Visualizer * visualizer;
    AudioSampler * audio_sampler;
};

#endif
