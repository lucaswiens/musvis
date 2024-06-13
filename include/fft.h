#ifndef FFT_H
#define FFT_H

#include <algorithm>
#include <complex>
#include <raylib.h>
#include <vector>

class FFT {
  public:
    FFT(const size_t &n, const size_t &width, const size_t &height);
    void Draw(const size_t &width, const size_t &height);
    void FastFourierTransformation(const bool &is_empty_track_list); // wrapper for fft
    void SetWave(const size_t i, const float &w) { waves.at(i) = w; }

    size_t GetN() { return n; }
    float GetWave(const size_t &i) { return std::abs(waves.at(i)); }
    float GetWaveWithDecayingAmplitude(const size_t &i);
    float GetFrequency(const size_t &i) { return std::abs(frequencies.at(i)); }

  private:
    size_t n, rectangle_width, wave_width;
    int x_offset, y_offset;
    float max_amplitude;
    std::vector<std::complex<float>> waves;
    std::vector<std::complex<float>> frequencies;
    std::vector<float> update_timer, previous_frequencies;

    void fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride);
};

#endif // !FFT_H
