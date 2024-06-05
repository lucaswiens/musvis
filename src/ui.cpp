#include "ui.h"

UserInterface::UserInterface(const KeyboardKey &pause_key) : pause_key(pause_key), border_offset(5), button_offset(5), track_list_height(40), track_list_width(3000 / 8 - 50), track_name_length(28) {}

void UserInterface::Draw() {
    for (size_t i = 0; i < track_list.size(); i++) {
        std::string track = track_list.at(i).substr(track_list.at(i).rfind('/') + 1, track_name_length);
        track = track.size() < track_name_length ? track : track + "...";
        Color background = current_track == i ? BLUE : GRAY;
        DrawRectangle(border_offset + button_offset, border_offset + button_offset + track_list_height * i, track_list_width, track_list_height - button_offset, background);
        DrawText(track.c_str(), border_offset + track_list_height / 2, border_offset + button_offset + track_list_height * (i + 0.25), track_list_height / 2, BLACK);
    }
}

void UserInterface::CheckKeyPress(const Music &music) {
    // Stop Music
    if (IsKeyPressed(KEY_SPACE)) {
        StopMusicStream(music);
        PlayMusicStream(music);
        SetPause(false);
    }
    // Pause/Resume music playing
    if (IsKeyPressed(pause_key)) {
        SetPause(!IsPaused());
        if (IsPaused()) {
            PauseMusicStream(music);
        } else {
            ResumeMusicStream(music);
        }
    }
    // Go to next or previous track
    if (IsKeyPressed(KEY_N)) {
        NextTrack();
    }
    if (IsKeyPressed(KEY_M)) {
        PreviousTrack();
    }
}

void UserInterface::CheckFilesDropped() {
    if (IsFileDropped()) {
        FilePathList dropped_files = LoadDroppedFiles();
        for (size_t i = 0; i < dropped_files.count; i++) {
            AddTrack(dropped_files.paths[i]);
        }
        SetCurrentTrack(0);
        SetPause(false);
        UnloadDroppedFiles(dropped_files);
    }
}

void UserInterface::AddTrack(const std::string &track_name) {
    if (std::find(track_list.begin(), track_list.end(), track_name) == track_list.end()) {
        track_list.push_back(track_name);
    }
}

void UserInterface::RemoveTrack(const std::string &track_name) {
    std::vector<std::string>::iterator track_position = std::find(track_list.begin(), track_list.end(), track_name);
    if (track_position == track_list.end()) {
        track_list.erase(track_position);
    }
}

void UserInterface::NextTrack() {
    has_track_changed = !track_list.empty();
    if (current_track != track_list.size() - 1) {
        current_track++;
    } else {
        current_track = 0;
    }
}
void UserInterface::PreviousTrack() {
    has_track_changed = !track_list.empty();
    if (current_track != 0) {
        current_track--;
    } else {
        current_track = track_list.size() - 1;
    }
}

void UserInterface::SetCurrentTrack(const size_t &i) {
    has_track_changed = !track_list.empty();
    current_track = i;
}
