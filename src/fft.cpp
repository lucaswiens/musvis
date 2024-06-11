#include "fft.h"

MusicPlayer::MusicPlayer(const size_t &n, const size_t &width, const size_t &height) : n(n), rectangle_width(1), wave_width(width / 388), x_offset(width / 8), y_offset(height / 8), waves(std::vector<std::complex<float>>(n)), frequencies(std::vector<std::complex<float>>(n)), update_timer(std::vector<float>(n)), previous_frequencies(std::vector<float>(n)) {
    shader = LoadShader(0, "resources/shaders/sdf.fs");
}

float MusicPlayer::GetWaveWithDecayingAmplitude(const size_t &i) { return previous_frequencies.at(i); }

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

void MusicPlayer::Draw(const bool &track_list_is_empty, const size_t &width, const size_t &height) {
    x_offset = width / 8;
    y_offset = height / 8;

    if (track_list_is_empty) {
        return;
        //DrawText("Drag your music files here to start playing:", width / 2 - 250, height / 2, 40, BLACK);
    } else {
        const size_t max_frequency = std::min({width / rectangle_width, n});
        for (size_t i = 0; i * rectangle_width < width && i < n; i++) {
            const float &amp = std::log(GetWaveWithDecayingAmplitude(i)) * 100;
            float hue = 360 * static_cast<float>(i) / max_frequency;
            const float saturation = 1.0;
            const float value = 1.;
            Color frequency_color = ColorFromHSV(hue, saturation, value);
            DrawRectangle(rectangle_width * i + x_offset, height - y_offset - amp, rectangle_width, amp, frequency_color);
        }
    }
}
