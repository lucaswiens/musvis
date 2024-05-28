#include "fft.h"

MusicPlayer::MusicPlayer(const size_t &n) : n(n) {
    waves = std::vector<std::complex<float>>(n);
    frequencies = std::vector<std::complex<float>>(n);
    previous_frequencies = std::vector<float>(n);
    update_timer = std::vector<float>(n);
    has_track_changed = false;
}

void MusicPlayer::FastFourierTransformation() {
    // calls fft to ensure correct passing of pointer arguments
    fft(this->waves.data(), n, this->frequencies.data(), 1);
    // stores decaying amplitude to avoid unpleasant flickering
    for (size_t i = 0; i < n; i++) {
        previous_frequencies.at(i) = std::max({std::abs(frequencies.at(i)), previous_frequencies.at(i) * std::exp(-update_timer.at(i))});
        // previous_frequencies.at(i) = std::max({std::abs(frequencies.at(i)), previous_frequencies.at(i) * (1-2*update_timer.at(i))});
    }
}

void MusicPlayer::fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride) {
    if (wave_size <= 1) {
        frequencies[0] = waves[0];
        return;
    }

    fft(waves, wave_size / 2, frequencies, 2 * stride);
    fft(waves + stride, wave_size / 2, frequencies + wave_size / 2, 2 * stride);
    std::fill(update_timer.begin(), update_timer.end(), GetFrameTime() * 2);
    for (size_t i = 0; i < wave_size / 2; i++) {

        const float t = static_cast<float>(i) / wave_size;
        const std::complex<float> p = frequencies[i];
        const std::complex<float> q = std::exp(std::complex<float>(0, -2 * M_PI * t)) * frequencies[i + wave_size / 2];
        frequencies[i] = p + q;
        frequencies[i + wave_size / 2] = p - q;
    }
}

void MusicPlayer::AddTrack(const std::string &track_name) {
    if (std::find(track_list.begin(), track_list.end(), track_name) == track_list.end()) {
        track_list.push_back(track_name);
    }
}

void MusicPlayer::RemoveTrack(const std::string &track_name) {
    std::vector<std::string>::iterator track_position = std::find(track_list.begin(), track_list.end(), track_name);
    if (track_position == track_list.end()) {
        track_list.erase(track_position);
    }
}

float MusicPlayer::GetWaveWithDecayingAmplitude(const size_t &i) { return previous_frequencies.at(i); }
