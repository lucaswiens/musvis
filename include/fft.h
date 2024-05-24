#ifndef FFT_H
#define FFT_H

#include <algorithm>
#include <complex>
#include <raylib.h>
#include <vector>

class MusicPlayer {
  public:
    MusicPlayer(const size_t &n);
    void FastFourierTransformation(); // wrapper for fft

    bool TrackListIsEmpty() { return track_list.empty(); };
    void AddTrack(const std::string &track_name);
    void RemoveTrack(const std::string &track_name);

    size_t GetN() { return n; }
    size_t GetNumberOfTracks() { return track_list.size(); }
    std::string GetCurrentTrack() { return track_list.at(current_track).c_str(); }
    float GetWave(const size_t &i) { return std::abs(waves.at(i)); }
    float GetFrequency(const size_t &i) { return std::abs(frequencies.at(i)); }
    bool HasTrackChanged() { return has_track_changed; };

    void SetTrackChangeToFalse() { has_track_changed = false; }
    void SetWave(const size_t i, const float &w) { waves.at(i) = w; }
    void SetCurrentTrack(const size_t &i) {
        has_track_changed = !track_list.empty();
        current_track = i;
    }
    void NextTrack() {
        has_track_changed = !track_list.empty();
        if (current_track != track_list.size() - 1) {
            current_track++;
        } else {
            current_track = 0;
        }
    }
    void PreviousTrack() {
        has_track_changed = !track_list.empty();
        if (current_track != 0) {
            current_track--;
        } else {
            current_track = track_list.size() - 1;
        }
    }

  private:
    bool has_track_changed;
    size_t n, current_track;
    std::vector<std::complex<float>> waves;
    std::vector<std::complex<float>> frequencies;
    std::vector<std::string> track_list;

    void fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride);
};

#endif // !FFT_H
