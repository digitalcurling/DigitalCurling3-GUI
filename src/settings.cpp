#include "settings.hpp"

#include <QSettings>
#include <QString>

namespace digitalcurling3_gui {

namespace {
    const auto kKeyAnimationSpeed = "animation_speed";
    const double kDefaultAnimationSpeed = 1.0;
    const auto kKeyShowGameResult = "show_game_result";
    const bool kDefaultShowGameResult = false;
} // unnamed namespace

void Settings::load()
{
    QSettings setting;
    animation_speed = setting.value(kKeyAnimationSpeed, kDefaultAnimationSpeed).toDouble();
    show_game_result = setting.value(kKeyShowGameResult, kDefaultShowGameResult).toBool();
}

void Settings::save()
{
    QSettings setting;
    setting.setValue(kKeyAnimationSpeed, animation_speed);
    setting.setValue(kKeyShowGameResult, show_game_result);
    setting.sync();
}

} // namespace digitalcurling3_gui::settings
