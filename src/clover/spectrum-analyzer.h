// This class describes a utility that analyzes audio streams and 
// provides snapshots of frequency distributions

#ifndef CLOVER_SPECTRUM_ANALYZER_H
#define CLOVER_SPECTRUM_ANALYZER_H

#include <jack/jack.h>

class SpectrumAnalyzer {
  public:
    double * histogram(double * fftw_in);
    void fft(jack_nframes_t nframes, double * fftw_in, double * fftw_out);
};

#endif
