#include <iostream>
#include <raylib.h>

// FFT is global instance because passing a member callback function is a hassle with the interface of raylib
MusicPlayer player(1 << 14, 3000, 1080);

void callback(void *bufferData, unsigned int frames) {
    float(*fs)[2] = static_cast<float(*)[2]>(bufferData);

    for (unsigned int i = 0; i < frames; ++i) {
        player.SetWave(i, fs[i][0]);
    }
}

int main(int argc, char **argv) {
    InitWindow(player.GetWidth(), player.GetHeight(), "MusVis");
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
        if (!player.TrackListIsEmpty() && !pause) {
            UpdateMusicStream(music);
            AttachAudioStreamProcessor(music.stream, callback);
            player.FastFourierTransformation();
        }

        BeginDrawing();
        player.Draw();
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
