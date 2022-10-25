#ifndef DIGITALCURLING3_GUI_SHOT_LOG_HPP
#define DIGITALCURLING3_GUI_SHOT_LOG_HPP

#include <cstdint>
#include <string_view>
#include <array>
#include <vector>
#include <chrono>
#include <optional>
#include <filesystem>
#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_gui {

class ShotLog
{
public:
    ShotLog(std::filesystem::path const& file_name);

    float secondsPerFrame() const { return seconds_per_frame_; }

    // シミュレーション座標系
    std::vector<digitalcurling3::GameState::Stones> const& frames() const { return frames_; }

    bool isAnimationLengthZero() const;

    // アニメーション全体の秒数
    float animationLength() const;

    // シミュレーション座標系
    digitalcurling3::GameState::Stones const& stonesAtFinish() const { return finish_; }

private:
    float seconds_per_frame_;
    std::vector<digitalcurling3::GameState::Stones> frames_;
    digitalcurling3::GameState::Stones finish_;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SHOT_LOG_HPP
