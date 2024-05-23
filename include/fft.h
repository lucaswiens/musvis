#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>

class FFT {
public:
  FFT(const size_t &n);
  void FastFourierTransformation(); // wrapper for fft
  std::vector<std::complex<float>> waves;
  std::vector<std::complex<float>> frequencies;
  size_t n;
private:
  void fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride);
};

#endif // !FFT_H
