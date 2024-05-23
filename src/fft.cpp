#include "fft.h"

FFT::FFT(const size_t &n) : n(n) {
  waves = std::vector<std::complex<float>>(n);
  frequencies = std::vector<std::complex<float>>(n);
  //for (int i = 0; i < n; i++) {
  //  const float t = static_cast<float>(i) / n;
  //  waves.at(i) = std::sin(2 * M_PI * t * 1);
  //  waves.at(i) += std::sin(2 * M_PI * t * 3);
  //}
}

void FFT::FastFourierTransformation() {
  // calls fft to ensure correct passing of pointer arguments
  fft(this->waves.data(), n, this->frequencies.data(), 1);
}

void FFT::fft(std::complex<float> *waves, const size_t &wave_size,
              std::complex<float> *frequencies, const int &stride) {
  if (wave_size <= 1) {
    frequencies[0] = waves[0];
    return;
  }

  fft(waves, wave_size / 2, frequencies, 2 * stride);
  fft(waves + stride, wave_size / 2, frequencies + wave_size / 2, 2 * stride);

  for (size_t i = 0; i < wave_size / 2; i++) {
    const float t = static_cast<float>(i) / wave_size;
    const std::complex<float> p = frequencies[i];
    const std::complex<float> q =
        std::exp(std::complex<float>(0, -2 * M_PI * t)) *
        frequencies[i + wave_size / 2];
    frequencies[i] = p + q;
    frequencies[i + wave_size / 2] = p - q;
  }
}

/*
FFT::InitializeTest() {
    for (int i = 0; i < n; i++) {
        const float t = static_cast<float>(i)/n;
        wave.at(i) = std::sin(2*M_PI * t * 1);
        wave.at(i) += std::sin(2*M_PI * t * 3);
    }
}
*/
