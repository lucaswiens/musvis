#include <raylib.h>

#include "fft.h"
#include "ui.h"

// FFT is global instance because passing a member callback function is a hassle with the interface of raylib
FFT visualizer(1 << 13, 3000, 1080);

void callback(void *bufferData, unsigned int frames) {
    float(*fs)[2] = static_cast<float(*)[2]>(bufferData);
    for (unsigned int i = 0; i < frames; ++i) {
        visualizer.SetWave(i, fs[i][0]); // currently only supports mono visualisation
    }
}

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    size_t factor = 2 * 135;
    InitWindow(factor * 16, factor * 9, "Musializer");
    { // load window icons
        const char *file_path = "./resources/icon/musvis.png";
        Image logo = LoadImage(GetFileExtension(file_path));
        SetWindowIcon(logo);
        UnloadImage(logo);
    }

    InitAudioDevice();
    SetTargetFPS(30);
    SetMasterVolume(0.5f);

    UserInterface ui(KEY_P, KEY_SPACE, KEY_N, KEY_M);
    ui.SetPause(argc > 1);

    Music music;
    music.looping = false;

    // Add arguments to track list
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '/') {
            ui.AddTrack(std::string(GetWorkingDirectory()) + '/' + std::string(argv[i]));
        } else {
            ui.AddTrack(argv[i]);
        }
        ui.SetCurrentTrack(0);
        ui.SetPause(false); // pause = false;
    }

    size_t width = GetScreenWidth();
    size_t height = GetScreenHeight();

    SetExitKey(KEY_Q);
    while (!WindowShouldClose()) {
        width = GetScreenWidth();
        height = GetScreenHeight();

        ui.CheckKeyPress(music);
        ui.CheckFilesDropped();
        ui.CheckEvents();

        if (ui.HasTrackChanged()) {
            music = LoadMusicStream(ui.GetCurrentTrack().c_str());
            PlayMusicStream(music);
            ui.SetTrackChangeToFalse(static_cast<float>(music.frameCount) / music.stream.sampleRate);
        }

        // Update Music Stream and read wave from stream
        if (!ui.TrackListIsEmpty() && !ui.IsPaused()) {
            UpdateMusicStream(music);
            AttachAudioStreamProcessor(music.stream, callback);
        }

        visualizer.FastFourierTransformation(ui.TrackListIsEmpty());

        BeginDrawing();
        ClearBackground(DARKGRAY);
        ui.Draw(width, height);
        visualizer.Draw(ui.TrackListIsEmpty(), width, height);
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
