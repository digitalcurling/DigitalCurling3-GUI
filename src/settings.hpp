#ifndef DIGITALCURLING3_GUI_SETTINGS_HPP
#define DIGITALCURLING3_GUI_SETTINGS_HPP

namespace digitalcurling3_gui {

struct Settings {
    double animation_speed;
    bool show_game_result;

    void load();
    void save();
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SETTINGS_HPP
