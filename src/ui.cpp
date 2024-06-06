#include "ui.h"

UserInterface::UserInterface(const KeyboardKey &pause_key, const KeyboardKey &stop_key, const KeyboardKey &next_key, const KeyboardKey &previous_key) : pause_key(pause_key), stop_key(stop_key), next_key(next_key), previous_key(previous_key), border_offset(5), button_offset(5), button_size(40), track_list_height(40), track_list_width(3000 / 8 - 50), track_name_length(28) {
    x_offset = 3000 / 8;
    y_offset = 1080 / 8;
    pause_button = {button_offset + x_offset, border_offset + button_offset + 7 * 1080 / 8, button_size - button_offset, button_size - button_offset};
    stop_button = {button_offset + x_offset + (2 * button_offset + button_size), border_offset + button_offset + 7 * 1080 / 8, button_size - button_offset, button_size - button_offset};
    previous_button = {button_offset + x_offset + 2 * (2 * button_offset + button_size), border_offset + button_offset + 7 * 1080 / 8, button_size - button_offset, button_size - button_offset};
    next_button = {button_offset + x_offset + 3 * (2 * button_offset + button_size), border_offset + button_offset + 7 * 1080 / 8, button_size - button_offset, button_size - button_offset};
}

void UserInterface::Draw() {
    // Track List
    for (size_t i = 0; i < track_list.size(); i++) {
        std::string track = track_list.at(i).substr(track_list.at(i).rfind('/') + 1, track_name_length);
        track = track.size() < track_name_length ? track : track + "...";
        Color background = current_track == i ? BLUE : track_hover == i ? LIGHTGRAY : GRAY;
        DrawRectangleRec(track_list_buttons.at(i), background);
        DrawText(track.c_str(), border_offset + track_list_height / 2, border_offset + button_offset + track_list_height * (i + 0.25), track_list_height / 2, BLACK);
    }
    // Buttons
    DrawRectangleRec(pause_button, pause_hover ? LIGHTGRAY : GRAY);
    DrawRectangleRec(stop_button, stop_hover ? LIGHTGRAY : GRAY);
    DrawRectangleRec(next_button, next_hover ? LIGHTGRAY : GRAY);
    DrawRectangleRec(previous_button, previous_hover ? LIGHTGRAY : GRAY);
}

void UserInterface::CheckKeyPress(const Music &music) {
    mouse_position = GetMousePosition();
    pause_hover = CheckCollisionPointRec(mouse_position, pause_button);
    stop_hover = CheckCollisionPointRec(mouse_position, stop_button);
    next_hover = CheckCollisionPointRec(mouse_position, next_button);
    previous_hover = CheckCollisionPointRec(mouse_position, previous_button);
    track_hover = 999;
    for (size_t i = 0; i < track_list_buttons.size(); i++) {
        if (CheckCollisionPointRec(mouse_position, track_list_buttons.at(i))) {
            track_hover = i;
            break;
        }
    }

    if (IsKeyPressed(stop_key) || (stop_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        StopMusicStream(music);
        PlayMusicStream(music);
        SetPause(false);
    } else if (IsKeyPressed(pause_key) || (pause_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        SetPause(!IsPaused());
        if (IsPaused()) {
            PauseMusicStream(music);
        } else {
            ResumeMusicStream(music);
        }
    } else if (IsKeyPressed(next_key) || (next_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        NextTrack();
    } else if (IsKeyPressed(previous_key) || (previous_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        PreviousTrack();
    } else if (track_hover < 999 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        SetCurrentTrack(track_hover);
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
        track_list_buttons.push_back({border_offset + button_offset, border_offset + button_offset + track_list_height * track_list.size(), track_list_width, track_list_height - button_offset});
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
