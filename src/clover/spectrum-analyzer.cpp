#include "spectrum-analyzer.h"

// Returns a histogram that visually represents a FFT snapshot of audio data.
// Bins are ordered by Frequency / FFT size.
// To accurately represent the range of human hearing, there are 512 bins
// that correspond to interesting frequencies.
double * SpectrumAnalyzer::histogram(double * fftw_in) {
  int i;
  double * res;
  for(i = 0; i < 512; i++) {
    res[i] = fftw_in[(i * 44100 / 1024)];
  }

  return res;
}
