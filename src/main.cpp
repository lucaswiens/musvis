#include <iostream>
#include <raylib.h>

#include "fft.h"

// FFT is global instance because passing a member callback function is a hassle with the style interface of raylib
MusicPlayer player(1 << 14);

void callback(void *bufferData, unsigned int frames) {
    float(*fs)[2] = static_cast<float(*)[2]>(bufferData);

    for (unsigned int i = 0; i < frames; ++i) {
        player.SetWave(i, fs[i][0]);
    }
}

int main() {
    const int width = 1920;
    const int height = 1080;

    InitWindow(width, height, "MusVis");
    InitAudioDevice();
    SetTargetFPS(60);
    SetMasterVolume(0.5f);

    bool pause = true;
    size_t rectangle_width = 1;
    size_t wave_width = width / 388;

    Music music;
    music.looping = false;

    SetExitKey(KEY_Q);
    while (!WindowShouldClose()) {
        // Stop Music
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
        // Go to next or previous track
        if (IsKeyPressed(KEY_N)) {
            player.NextTrack();
        }
        if (IsKeyPressed(KEY_M)) {
            player.PreviousTrack();
        }

        if (IsFileDropped()) {
            FilePathList dropped_files = LoadDroppedFiles();
            for (size_t i = 0; i < dropped_files.count; i++) {
                player.AddTrack(dropped_files.paths[i]);
            }
            player.SetCurrentTrack(0); // hardcoded for now
            pause = false;
            UnloadDroppedFiles(dropped_files);
        }

        if (player.HasTrackChanged()) {
            music = LoadMusicStream(player.GetCurrentTrack().c_str());
            PlayMusicStream(music);
            player.SetTrackChangeToFalse();
        }

        // Update Music Stream and read wave from stream
        if (!player.TrackListIsEmpty()) {
            UpdateMusicStream(music);
            AttachAudioStreamProcessor(music.stream, callback);
            player.FastFourierTransformation();
        }

        BeginDrawing();
        if (player.TrackListIsEmpty()) {
            // center propery..
            DrawText("Drag your music files here to start playing:", width / 2 - 250, height / 2, 40, BLACK);
        } else {
            for (size_t i = 0; i * rectangle_width < width and i < player.GetN(); i++) {
                // Draw wave - probably remove this later
                const float &wave_amp = player.GetWave(i) * 500;
                DrawRectangle(wave_width * i, height / 4 - wave_amp + 1, wave_width, wave_amp, WHITE);
                DrawRectangle(wave_width * i, height / 4, wave_width, wave_amp, WHITE);

                // Draw amplitude of each frequency up to ~16k or however many fit
                // on screen
                const float &amp = player.GetFrequency(i) * 5;
                DrawRectangle(rectangle_width * i, height - amp, rectangle_width, amp, RED);
            }
        }

        ClearBackground(DARKGRAY);

        DrawRectangle(20, 20, 425, 100, WHITE);
        // DrawRectangleLines(20, 20, 425, 145, GRAY);
        DrawText("PRESS SPACE TO RESTART MUSIC", 40, 40, 20, BLACK);
        DrawText("PRESS P TO PAUSE/RESUME", 40, 70, 20, BLACK);

        EndDrawing();
    }

    UnloadMusicStream(music); // Unload music stream buffers from RAM
    CloseAudioDevice();       // Close audio device (music streaming is automatically
                              // stopped)
    CloseWindow();

    return 0;
}
