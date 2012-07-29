#include <stdio.h>
#include <sndfile.h>
#include <aubio/aubio.h>
#include <freej.h>

typedef struct _aubio_sndfile_t aubio_sndfile_t;
struct _aubio_sndfile_t {
  SNDFILE *handle;
  int samplerate;
  int channels;
  int format;
  float *tmpdata;
  int size;
};

int main() {
  aubio_sndfile_t * f = AUBIO_NEW(aubio_sndfile_t);
  return 0;
}
