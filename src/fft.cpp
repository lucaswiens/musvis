#include "fft.h"

MusicPlayer::MusicPlayer(const size_t &n, const size_t &width, const size_t &height) : n(n), width(width), height(height) {
    rectangle_width = 2;
    wave_width = width / 388;
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

void MusicPlayer::Draw() {
    if (TrackListIsEmpty()) {
        // center propery..
        DrawText("Drag your music files here to start playing:", width / 2 - 250, height / 2, 40, BLACK);
    } else {
        const size_t max_frequency = std::min({width / rectangle_width, n});
        for (size_t i = 0; i * rectangle_width <= width && i < n; i++) {
            // Draw wave - probably remove this later
            const float &wave_amp = GetWave(i) * 500;
            DrawRectangle(wave_width * i, height / 4 - wave_amp + 1, wave_width, wave_amp, GRAY);
            DrawRectangle(wave_width * i, height / 4, wave_width, wave_amp, GRAY);

            const float &amp = GetWaveWithDecayingAmplitude(i) * 5;
            float hue = 360 * static_cast<float>(i) / max_frequency;
            const float saturation = 1.0;
            const float value = 1.;
            Color frequency_color = ColorFromHSV(hue, saturation, value);
            DrawRectangle(rectangle_width * i, height - amp, rectangle_width, amp, frequency_color);
        }
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

void MusicPlayer::NextTrack() {
    has_track_changed = !track_list.empty();
    if (current_track != track_list.size() - 1) {
        current_track++;
    } else {
        current_track = 0;
    }
}
void MusicPlayer::PreviousTrack() {
    has_track_changed = !track_list.empty();
    if (current_track != 0) {
        current_track--;
    } else {
        current_track = track_list.size() - 1;
    }
}

void MusicPlayer::SetCurrentTrack(const size_t &i) {
    has_track_changed = !track_list.empty();
    current_track = i;
}
