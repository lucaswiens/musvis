#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <algorithm>
#include <complex>
#include <raylib.h>
#include <vector>

class MusicPlayer {
  public:
    MusicPlayer(const size_t &n, const size_t &width, const size_t &height);
    void FastFourierTransformation(); // wrapper for fft

    bool TrackListIsEmpty() { return track_list.empty(); };
    void AddTrack(const std::string &track_name);
    void RemoveTrack(const std::string &track_name);

    size_t GetN() { return n; }
    size_t GetWidth() { return width; }
    size_t GetHeight() { return height; }
    size_t GetNumberOfTracks() { return track_list.size(); }
    std::string GetCurrentTrack() { return track_list.at(current_track).c_str(); }
    float GetWave(const size_t &i) { return std::abs(waves.at(i)); }
    float GetWaveWithDecayingAmplitude(const size_t &i);
    float GetFrequency(const size_t &i) { return std::abs(frequencies.at(i)); }
    bool HasTrackChanged() { return has_track_changed; };

    void SetTrackChangeToFalse() { has_track_changed = false; }
    void SetWave(const size_t i, const float &w) { waves.at(i) = w; }
    void SetCurrentTrack(const size_t &i);

    void NextTrack();
    void PreviousTrack();
    void Draw();

  private:
    bool has_track_changed;
    size_t n, current_track, width, height, rectangle_width, wave_width;
    std::vector<std::complex<float>> waves;
    std::vector<std::complex<float>> frequencies;
    std::vector<std::string> track_list;
    std::vector<float> update_timer, previous_frequencies;

    void fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride);
};

#endif // !MUSICPLAYER_H
