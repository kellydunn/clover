#include <stdio.h>
#include <sndfile.h>
#include <aubio/aubio.h>
#include <aubio/sample.h>
#include <aubio/sndfileio.h>
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

int main(int argc, char **argv) {
  SF_INFO sf_info;
  SNDFILE * file = sf_open("hustle_main_sample.wav", SFM_READ, &sf_info);
  sf_count_t frames;
  float *tmpdata;
  smpl_t *pread;
  sf_read_float(file, tmpdata, 1024);
  printf("%d\n", sizeof(tmpdata));
  return 0;
}
