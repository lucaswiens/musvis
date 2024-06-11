#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <algorithm>
#include <complex>
#include <raylib.h>
#include <vector>

class MusicPlayer {
  public:
    MusicPlayer(const size_t &n, const size_t &width, const size_t &height);
    void Draw(const bool &track_list_is_empty, const size_t &width, const size_t &height);
    void FastFourierTransformation(); // wrapper for fft
    void SetWave(const size_t i, const float &w) { waves.at(i) = w; }

    size_t GetN() { return n; }
    float GetWave(const size_t &i) { return std::abs(waves.at(i)); }
    float GetWaveWithDecayingAmplitude(const size_t &i);
    float GetFrequency(const size_t &i) { return std::abs(frequencies.at(i)); }

  private:
    size_t n, rectangle_width, wave_width;
    int x_offset, y_offset;
    std::vector<std::complex<float>> waves;
    std::vector<std::complex<float>> frequencies;
    std::vector<float> update_timer, previous_frequencies;

    void fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride);
};

#endif // !MUSICPLAYER_H
