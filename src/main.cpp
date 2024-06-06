#include <iostream>
#include <raylib.h>

#include "fft.h"
#include "ui.h"

// FFT is global instance because passing a member callback function is a hassle with the interface of raylib
MusicPlayer player(1 << 13, 3000, 1080);

void callback(void *bufferData, unsigned int frames) {
    float(*fs)[2] = static_cast<float(*)[2]>(bufferData);
    for (unsigned int i = 0; i < frames; ++i) {
        player.SetWave(i, fs[i][0]); // currently only supports mono visualisation
    }
}

int main(int argc, char **argv) {
    UserInterface ui(KEY_P, KEY_SPACE, KEY_N, KEY_M);
    InitWindow(player.GetWidth(), player.GetHeight(), "MusVis");
    InitAudioDevice();
    SetTargetFPS(60);
    SetMasterVolume(0.5f);

    ui.SetPause(argc > 1);

    Music music;
    music.looping = false;

    // Add arguments to track list
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '/') {
            // std::cout << std::string(GetWorkingDirectory()) + '/' + std::string(argv[i]) << std::endl;
            ui.AddTrack(std::string(GetWorkingDirectory()) + '/' + std::string(argv[i]));
        } else {
            ui.AddTrack(argv[i]);
        }
        // ui.SetTrackChangeToFalse();
        ui.SetCurrentTrack(0);
        ui.SetPause(false); // pause = false;
    }

    SetExitKey(KEY_Q);
    while (!WindowShouldClose()) {
        ui.CheckKeyPress(music);
        ui.CheckFilesDropped();
        if (ui.HasTrackChanged()) {
            music = LoadMusicStream(ui.GetCurrentTrack().c_str());
            PlayMusicStream(music);
            ui.SetTrackChangeToFalse();
        }

        // Update Music Stream and read wave from stream
        if (!ui.TrackListIsEmpty() && !ui.IsPaused()) {
            UpdateMusicStream(music);
            AttachAudioStreamProcessor(music.stream, callback);
            player.FastFourierTransformation();
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        ui.Draw();
        player.Draw(ui.TrackListIsEmpty());
        EndDrawing();
    }

    UnloadMusicStream(music); // Unload music stream buffers from RAM
    CloseAudioDevice();       // Close audio device (music streaming is automatically stopped)
    CloseWindow();

    return 0;
}
