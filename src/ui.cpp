#include "ui.h"
#include <raylib.h>

UserInterface::UserInterface(const KeyboardKey &pause_key, const KeyboardKey &stop_key, const KeyboardKey &next_key, const KeyboardKey &previous_key)
    : pause_key(pause_key), stop_key(stop_key), next_key(next_key), previous_key(previous_key), pause_hover(false), stop_hover(false), previous_hover(false), next_hover(false), volume_hover(false), border_offset(10), button_offset(5), button_size(40), track_list_height(40), track_list_width(375), track_name_length(28), volume(GetMasterVolume()) {
    int file_size = 0;
    unsigned char *file_data = LoadFileData(font_path.c_str(), &file_size);
    font.glyphs = LoadFontData(file_data, file_size, 32, 0, 0, FONT_SDF);
    atlas = GenImageFontAtlas(font.glyphs, &font.recs, 95, 32, 0, 1);
    font.texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR); // Required for SDF font
    shader = LoadShader(0, "resources/shaders/sdf.fs");
}

void UserInterface::Draw(const size_t &width, const size_t &height) {
    x_offset = width / 8.0f;
    y_offset = height / 8.0f;

    const float horizontal_border_position = (width - x_offset - button_offset - border_offset);
    const float vertical_border_position = 7.0f * height / 8.0f;

    time_position += !pause * GetFrameTime();
    std::string timer_label = ConvertSecondToTimeLabel(time_position) + " / " + track_duration_label;
    Vector2 time_label_length = MeasureTextEx(font, timer_label.c_str(), 45, 1.0f);

    track_list_width = x_offset - 50;
    track_name_length = static_cast<int>(track_list_width / 12);

    // Track List
    BeginShaderMode(shader);
    if (track_list.empty()) {
        std::string empty_track_list_instructions = "Drag your music files here to start playing:";
        Vector2 empty_track_list_instructions_length = MeasureTextEx(font, empty_track_list_instructions.c_str(), 65, 1.0f);

        DrawTextEx(font, empty_track_list_instructions.c_str(), {(width - empty_track_list_instructions_length.x) / 2.0f, height / 2.0f}, 65, 1.0f, BLACK);
    } else {
        for (size_t i = 0; i < track_list.size(); i++) {
            track_list_buttons.at(i) = {border_offset + button_offset, border_offset + button_offset + track_list_height * i, track_list_width, track_list_height - button_offset};

            std::string track = track_list.at(i).substr(track_list.at(i).rfind('/') + 1, track_name_length);
            track = track.size() < track_name_length ? track : track + "...";
            Color background = current_track == i ? BLUE : track_hover == i ? LIGHTGRAY : GRAY;
            DrawRectangleRec(track_list_buttons.at(i), background);
            const Vector2 position = {border_offset + track_list_height / 2.0f, border_offset + button_offset + track_list_height * (i + 0.25f)};
            DrawTextEx(font, track.c_str(), position, track_list_height / 1.5f, 1.0, BLACK);
            // int codepoint_size = codepoints.at(i).size() < track_name_length ? codepoints.at(i).size() : track_name_length;
            // DrawTextCodepoints(font, codepoints.at(i).data(), codepoint_size, position, track_list_height / 1.5f, 5, BLACK);
        }

        track_bar = {button_offset + x_offset, vertical_border_position + button_size + border_offset + button_offset, horizontal_border_position, button_size - button_offset};
        DrawRectangleRec(track_bar, LIGHTGRAY);
        DrawRectangleRec({track_bar.x, track_bar.y, time_position / track_duration * track_bar.width, track_bar.height}, BLUE);

        DrawTextEx(font, timer_label.c_str(), {track_bar.x + track_bar.width - time_label_length.x - border_offset, border_offset + button_size + vertical_border_position}, 45, 1.0f, BLACK);

        pause_button = {button_offset + x_offset, vertical_border_position + border_offset + button_offset, button_size - button_offset, button_size - button_offset};
        stop_button = {pause_button.x + button_offset + button_size, pause_button.y, pause_button.width, pause_button.height};
        previous_button = {stop_button.x + button_offset + button_size, pause_button.y, pause_button.width, pause_button.height};
        next_button = {previous_button.x + button_offset + button_size, pause_button.y, pause_button.width, pause_button.height};

        DrawRectangleRec(pause_button, pause_hover ? LIGHTGRAY : GRAY);
        DrawRectangleRec(stop_button, stop_hover ? LIGHTGRAY : GRAY);
        DrawRectangleRec(previous_button, previous_hover ? LIGHTGRAY : GRAY);
        DrawRectangleRec(next_button, next_hover ? LIGHTGRAY : GRAY);

        DrawTextEx(font, IsPaused() ? "p" : "P", {pause_button.x + button_size * 0.125f, pause_button.y - button_size * 0.125f}, 45, 1.0f, BLACK);
        DrawTextEx(font, "S", {stop_button.x + button_size * 0.125f, stop_button.y - button_size * 0.125f}, 45, 1.0f, BLACK);
        DrawTextEx(font, "<", {previous_button.x + button_size * 0.125f, previous_button.y - button_size * 0.25f}, 45, 1.0f, BLACK);
        DrawTextEx(font, ">", {next_button.x + button_size * 0.125f, next_button.y - button_size * 0.25f}, 45, 1.0f, BLACK);

        volume_bar = {next_button.x + button_offset + button_size, pause_button.y, pause_button.width * (1 + volume_hover * 4), pause_button.height};
        DrawRectangleRec(volume_bar, GRAY);
        if (volume_hover) {
            DrawRectangle(volume_bar.x + volume * volume_bar.width, volume_bar.y, 10, volume_bar.height, BLUE);
        } else {
            DrawTextEx(font, "V", {volume_bar.x + button_size * 0.125f, volume_bar.y - button_size * 0.125f}, 45, 1.0f, BLACK);
        }
    }
    EndShaderMode();
}

void UserInterface::CheckKeyPress(const Music &music) {
    mouse_position = GetMousePosition();

    pause_hover = CheckCollisionPointRec(mouse_position, pause_button);
    stop_hover = CheckCollisionPointRec(mouse_position, stop_button);
    previous_hover = CheckCollisionPointRec(mouse_position, previous_button);
    next_hover = CheckCollisionPointRec(mouse_position, next_button);
    volume_hover = CheckCollisionPointRec(mouse_position, volume_bar) || (volume_hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT));

    track_hover = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < track_list_buttons.size(); i++) {
        if (CheckCollisionPointRec(mouse_position, track_list_buttons.at(i))) {
            track_hover = i;
            break;
        }
    }

    if (CheckCollisionPointRec(mouse_position, track_bar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        time_position = (mouse_position.x - track_bar.x) / track_bar.width * track_duration;
        SeekMusicStream(music, time_position);
    }

    if (IsKeyPressed(stop_key) || (stop_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        StopMusicStream(music);
        PlayMusicStream(music);
        SetPause(false);
    } else if (IsKeyPressed(pause_key) || (pause_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        pause = !IsPaused();
        if (IsPaused()) {
            PauseMusicStream(music);
        } else {
            ResumeMusicStream(music);
        }
    } else if (IsKeyPressed(previous_key) || (previous_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        PreviousTrack();
    } else if (IsKeyPressed(next_key) || (next_hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
        NextTrack();
    } else if (volume_hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        volume = std::max({0.0f, std::min({1.0f, (mouse_position.x - volume_bar.x) / volume_bar.width})});
        SetMasterVolume(volume);
    } else if (track_hover < std::numeric_limits<size_t>::max() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        SetCurrentTrack(track_hover);
    } else if (track_hover < std::numeric_limits<size_t>::max() && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        RemoveTrack(track_hover);
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

void UserInterface::CheckEvents() {
    if (time_position > track_duration) {
        NextTrack();
    }
}

void UserInterface::AddTrack(const std::string &track) {
    const bool is_supported_file = IsFileExtension(track.c_str(), ".mp3") || IsFileExtension(track.c_str(), ".wav") || IsFileExtension(track.c_str(), ".ogg") || IsFileExtension(track.c_str(), ".ogg") || IsFileExtension(track.c_str(), ".qoa");
    if (is_supported_file && std::find(track_list.begin(), track_list.end(), track) == track_list.end()) {
        track_list_buttons.push_back({border_offset + button_offset, border_offset + button_offset + track_list_height * track_list.size(), track_list_width, track_list_height - button_offset});
        track_list.push_back(track);

        std::string track_name = track.substr(track.rfind('/') + 1);
        for (size_t i = 0; i < track_name.size(); i++) {
            int codepoint_byte_count = 0;
            int codepoint = GetCodepoint(&track_name[i], &codepoint_byte_count);
            if (std::find(codepoints.begin(), codepoints.end(), codepoint) != codepoints.end()) {
                codepoints.push_back(codepoint);
            }
        }
        font = LoadFontEx(font_path.c_str(), 32, codepoints.data(), codepoints.size());
        SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR); // Required for SDF font
    }
}

void UserInterface::RemoveTrack(const size_t &track_position) {
    if (track_position < track_list.size()) {
        track_list.erase(track_list.begin() + track_position);
        if (current_track == track_hover) {
            SetCurrentTrack(track_hover);
        } else if (track_position < current_track) {
            current_track--;
        }
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

UserInterface::~UserInterface() {
    UnloadShader(shader);
    UnloadFont(font);
}

std::string UserInterface::ConvertSecondToTimeLabel(const float &time) {
    std::string minutes = std::to_string(static_cast<int>(time / 60));
    int seconds = static_cast<int>(time) % 60;
    std::string seconds_label = (seconds < 10 ? "0" : "") + std::to_string(seconds);
    return minutes + ":" + seconds_label;
}
