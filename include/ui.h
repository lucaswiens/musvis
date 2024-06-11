#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <algorithm>
#include <raylib.h>
#include <string>
#include <vector>

class UserInterface {
  public:
    UserInterface(const KeyboardKey &pause_key, const KeyboardKey &stop_key, const KeyboardKey &next_key, const KeyboardKey &previous_key);
    ~UserInterface();
    void CheckKeyPress(const Music &music);
    void CheckFilesDropped();
    void AddTrack(const std::string &track);
    void RemoveTrack(const std::string &track);
    void NextTrack();
    void PreviousTrack();
    void Draw(const size_t &width, const size_t &height);

    void SetTrackChangeToFalse() { has_track_changed = false; }
    void SetCurrentTrack(const size_t &i);
    void SetPause(const bool &pause) { this->pause = pause; }

    bool TrackListIsEmpty() { return track_list.empty(); };
    size_t GetNumberOfTracks() { return track_list.size(); }
    std::string GetCurrentTrack() { return track_list.at(current_track).c_str(); }
    bool HasTrackChanged() { return has_track_changed; };
    bool IsPaused() { return pause; }

  private:
    Font font;
    std::string font_path = "resources/fonts/KleeOne-SemiBold.ttf";
    Shader shader;
    Image atlas;
    KeyboardKey pause_key, stop_key, next_key, previous_key;
    bool has_track_changed, pause, pause_hover = false, stop_hover = false, next_hover = false, previous_hover = false;
    size_t current_track, track_hover;
    float border_offset, button_offset, button_size, track_list_height, track_list_width, track_name_length, x_offset, y_offset;
    Vector2 mouse_position;
    Rectangle pause_button, stop_button, next_button, previous_button;
    std::vector<Rectangle> track_list_buttons;
    std::vector<std::string> track_list;
    std::vector<int> codepoints; 
};

#endif // !USERINTERFACE_H
