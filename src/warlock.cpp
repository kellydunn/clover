#include <stdio.h>
#include <sndfile.h>
#include <aubio/aubio.h>
#include <freej.h>

int main(int argc, char **argv) {
  SF_INFO sf_info;
  SNDFILE *file = sf_open("hustle_main_sample.wav", SFM_READ, &sf_info);
  sf_count_t frames;
  float *tmpdata;
  smpl_t *pread;
  sf_read_float(file, tmpdata, 1024);

  printf("%d\n", sizeof(pread));
  return 0;
}
