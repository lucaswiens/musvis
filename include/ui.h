#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <algorithm>
#include <cstring>
#include <raylib.h>
#include <string>
#include <vector>

class UserInterface {
  public:
    UserInterface(const KeyboardKey &pause_key);
    void CheckKeyPress(const Music &music);
    void CheckFilesDropped();
    void AddTrack(const std::string &track_name);
    void RemoveTrack(const std::string &track_name);
    void NextTrack();
    void PreviousTrack();
    void Draw();

    void SetTrackChangeToFalse() { has_track_changed = false; }
    void SetCurrentTrack(const size_t &i);
    void SetPause(const bool &pause) { this->pause = pause; }

    bool TrackListIsEmpty() { return track_list.empty(); };
    size_t GetNumberOfTracks() { return track_list.size(); }
    std::string GetCurrentTrack() { return track_list.at(current_track).c_str(); }
    bool HasTrackChanged() { return has_track_changed; };
    bool IsPaused() { return pause; }

  private:
    Font text_font;
    KeyboardKey pause_key;
    bool has_track_changed, pause;
    size_t current_track, border_offset, button_offset, track_list_height, track_list_width, track_name_length;
    std::vector<std::string> track_list;
};

#endif // !USERINTERFACE_H
