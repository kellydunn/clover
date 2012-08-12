#include "clover.h"

double window(clover_jack_t * jack, jack_default_audio_sample_t in, int n) {
  return .5 * (1 - cos(2*PI*n/(int)jack->frames)) * (double)in;
}
