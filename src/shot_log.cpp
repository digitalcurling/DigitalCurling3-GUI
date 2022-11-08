#include "shot_log.hpp"
#include <fstream>
#include <QDebug>
#include <QFile>
#include "coord.hpp"

namespace dc = digitalcurling3;

namespace digitalcurling3_gui {

namespace {

dc::GameState::Stones StonesFromJson(nlohmann::json const& j)
{
    dc::GameState::Stones stones;
    j.at("team0").get_to(stones[0]);
    j.at("team1").get_to(stones[1]);
    return stones;
}

} // unnamed namespace

ShotLog::ShotLog(QString const& file_name)
    : seconds_per_frame_(0.f)
    , frames_()
    , finish_()
{
    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("file open failed");
    }

    QByteArray bytes = file.readAll();
    std::string file_str = bytes.toStdString();

    nlohmann::json j = nlohmann::json::parse(file_str);

    auto const& j_log = j.at("log");
    auto const end = j_log.at("end").get<uint8_t>();
    auto const shot_side = dc::coordinate::GetShotSide(end);

    auto const& j_trajectory = j_log.at("trajectory");

    j_trajectory.at("seconds_per_frame").get_to(seconds_per_frame_);

    dc::GameState::Stones stones = StonesFromJson(j_trajectory.at("start"));
    frames_.emplace_back(TransformStones(stones, shot_side, dc::coordinate::Id::kSimulation));

    for (auto const& frame_diffs : j_trajectory.at("frames")) {
        // 差分を適用
        for (auto const& diff : frame_diffs) {
            auto const team = static_cast<size_t>(diff.at("team").get<dc::Team>());
            auto const index = diff.at("index").get<std::uint32_t>();
            stones[team][index] = diff.at("value").get<dc::GameState::Stones::value_type::value_type>();
        }

        // 記録
        frames_.emplace_back(TransformStones(stones, shot_side, dc::coordinate::Id::kSimulation));
    }

    finish_ = TransformStones(StonesFromJson(j_trajectory.at("finish")), shot_side, dc::coordinate::Id::kSimulation);

    if (frames_.size() == 0) {
        throw std::runtime_error("invalid format");
    }
}

bool ShotLog::isAnimationLengthZero() const
{
    return frames_.size() <= 1;
}

float ShotLog::animationLength() const
{
    assert(frames_.size() > 0);
    return static_cast<float>(frames_.size() - 1) * seconds_per_frame_;
}

} // namespace digitalcurling3_gui
