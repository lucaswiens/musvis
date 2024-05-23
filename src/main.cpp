#include<iostream>

#include <raylib.h>
#include "fft.h"

// FFT is global instance because passing a member callback function is a hassle with a C style interface..
FFT fft(1<<14);

void callback(void *bufferData, unsigned int frames)
{
    float (*fs)[2] = static_cast<float(*)[2]>(bufferData);

    for (unsigned int i = 0; i < frames; ++i) {
        fft.waves[i] = fs[i][0];
    }
}

int main() {
    const int width = 1920;
    const int height = 1080;

    InitWindow(width, height, "MusVis");
    InitAudioDevice();
    SetTargetFPS(30);
    SetMasterVolume(0.5f);

    Music music = LoadMusicStream("01.ツバサ.mp3");
    //Music music = LoadMusicStream("440hz.ogg");
    music.looping = false;
    PlayMusicStream(music);

    bool pause = false;
    std::cout <<
        "frame count = " << music.frameCount << std::endl <<
        "rate        = " << music.stream.sampleRate << std::endl <<
        "size        = " << music.stream.sampleSize << std::endl <<
        "channel     = " << music.stream.channels << std::endl;

    size_t rectangle_width = 1;
    size_t wave_width = width/388; // number of samples per frame

    while (!WindowShouldClose()) {
        // Keybindinds
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            StopMusicStream(music);
            PlayMusicStream(music);
            pause = false;
        }

        // Pause/Resume music playing
        if (IsKeyPressed(KEY_P)) {
            pause = !pause;

            if (pause) {
                PauseMusicStream(music);
            } else {
                ResumeMusicStream(music);
            }
        }

        // Update Music Stream and read wave from stream
        UpdateMusicStream(music);
        AttachAudioStreamProcessor(music.stream, callback);
        fft.FastFourierTransformation();

        BeginDrawing();
            for (size_t i = 0; i *rectangle_width < width and i < fft.n; i++) {
                // Draw wave - probably remove this later
                const float &wave_amp = std::abs(fft.waves[i]) * 500;
                DrawRectangle(wave_width * i, height/4 - wave_amp + 1, wave_width, wave_amp, WHITE);
                DrawRectangle(wave_width * i, height/4,                wave_width, wave_amp, WHITE);

                // Draw amplitude of each frequency up to ~16k or however many fit on screen
                const float &amp = std::abs(fft.frequencies[i])*5;
                DrawRectangle(rectangle_width * i, height - amp, rectangle_width, amp, RED);
            }

            ClearBackground(DARKGRAY);

            DrawRectangle(20, 20, 425, 100, WHITE);
            //DrawRectangleLines(20, 20, 425, 145, GRAY);
            DrawText("PRESS SPACE TO RESTART MUSIC", 40, 40, 20, BLACK);
            DrawText("PRESS P TO PAUSE/RESUME", 40, 70, 20, BLACK);

        EndDrawing();
   }

    UnloadMusicStream(music);          // Unload music stream buffers from RAM
    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
    CloseWindow();

    return 0;
}

