// This class describes a utility that analyzes audio streams and 
// provides snapshots of frequency distributions

#ifndef CLOVER_SPECTRUM_ANALYZER_H
#define CLOVER_SPECTRUM_ANALYZER_H

class SpectrumAnalyzer {
  public:
    double * histogram(double * fftw_in);
};

#endif
