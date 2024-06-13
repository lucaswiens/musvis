#include "fft.h"

FFT::FFT(const size_t &n, const size_t &width, const size_t &height) : n(n), rectangle_width(1), wave_width(width / 388), x_offset(width / 8), y_offset(height / 8), max_amplitude(100), waves(std::vector<std::complex<float>>(n)), frequencies(std::vector<std::complex<float>>(n)), update_timer(std::vector<float>(n)), previous_frequencies(std::vector<float>(n)) {}

float FFT::GetWaveWithDecayingAmplitude(const size_t &i) {
    max_amplitude = std::max({max_amplitude, *std::max(previous_frequencies.begin(), previous_frequencies.end())});
    return previous_frequencies.at(i);
}

void FFT::FastFourierTransformation() {
    // calls fft to ensure correct passing of pointer arguments
    fft(this->waves.data(), n, this->frequencies.data(), 1);
    // stores decaying amplitude to avoid unpleasant flickering
    const float frame_time = GetFrameTime();
    for (size_t i = 0; i < n; i++) {
        update_timer.at(i) += frame_time;
        float amplitude = std::abs(frequencies.at(i));
        float decaying_amplitude = std::abs(previous_frequencies.at(i)) * std::exp(-0.1 * update_timer.at(i));
        if (amplitude < decaying_amplitude) {
            previous_frequencies.at(i) = decaying_amplitude;
        } else {
            previous_frequencies.at(i) = amplitude;
            update_timer.at(i) = 0;
        }
    }
}

void FFT::fft(std::complex<float> *waves, const size_t &wave_size, std::complex<float> *frequencies, const int &stride) {
    if (wave_size <= 1) {
        frequencies[0] = waves[0];
        return;
    }

    fft(waves, wave_size / 2, frequencies, 2 * stride);
    fft(waves + stride, wave_size / 2, frequencies + wave_size / 2, 2 * stride);
    for (size_t i = 0; i < wave_size / 2; i++) {
        const float t = static_cast<float>(i) / wave_size;
        const std::complex<float> p = frequencies[i];
        const std::complex<float> q = std::exp(std::complex<float>(0, -2 * M_PI * t)) * frequencies[i + wave_size / 2];
        frequencies[i] = p + q;
        frequencies[i + wave_size / 2] = p - q;
    }
}

void FFT::Draw(const bool &track_list_is_empty, const size_t &width, const size_t &height) {
    x_offset = width / 8;
    y_offset = height / 8;

    const size_t max_frequency = std::min({width / rectangle_width, n});
    for (size_t i = 0; i * rectangle_width < width && i < n; i++) {
        const float &amp = 6.5f * y_offset * std::log(GetWaveWithDecayingAmplitude(i)) / std::log(max_amplitude);
        float hue = 360 * static_cast<float>(i) / max_frequency;
        const float saturation = 1.0;
        const float value = 1.;
        Color frequency_color = ColorFromHSV(hue, saturation, value);
        DrawRectangle(rectangle_width * i + x_offset, height - y_offset - amp, rectangle_width, amp, frequency_color);
    }
}
