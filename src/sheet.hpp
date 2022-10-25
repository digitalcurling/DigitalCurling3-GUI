#ifndef DIGITALCURLING3_GUI_SHEET_HPP
#define DIGITALCURLING3_GUI_SHEET_HPP

#include <QColor>
#include <QPainter>
#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_gui {

constexpr QColor kSheetBackgroundColor(200, 200, 200);
constexpr QColor kSheetColor(250, 250, 250);
constexpr QColor kInnerCircleColor(250, 170, 170);
constexpr QColor kOuterCircleColor(170, 170, 250);
constexpr QColor kLineColor(150, 150, 150);
constexpr QColor kStoneColor(100, 100, 100);
constexpr std::array<QColor, 2> kTeamColor{{ QColor(255, 0, 0), QColor(255, 255, 0) }};
constexpr std::array<QColor, 2> kTeamTrajectoryColor{{ QColor(150, 0, 0, 200), QColor(150, 150, 0, 200) }};

constexpr qreal kCenterLineX = digitalcurling3::coordinate::GetCenterLineX(digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kHogLine0Y = digitalcurling3::coordinate::GetHogLineY(false, digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kHogLine1Y = digitalcurling3::coordinate::GetHogLineY(true , digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kTeeLine0Y = digitalcurling3::coordinate::GetTeeLineY(false, digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kTeeLine1Y = digitalcurling3::coordinate::GetTeeLineY(true , digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kBackLine0Y = digitalcurling3::coordinate::GetBackLineY(false, digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kBackLine1Y = digitalcurling3::coordinate::GetBackLineY(true , digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kHack0Y = digitalcurling3::coordinate::GetHackY(false, digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kHack1Y = digitalcurling3::coordinate::GetHackY(true , digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kBackBoard0Y = digitalcurling3::coordinate::GetBackBoardY(false, digitalcurling3::coordinate::Id::kSimulation);
constexpr qreal kBackBoard1Y = digitalcurling3::coordinate::GetBackBoardY(true , digitalcurling3::coordinate::Id::kSimulation);

class ShotLog;

class SheetDrawer {
public:
    SheetDrawer();
    void reset();
    void prepare(ShotLog const* shot_log, float current_time, digitalcurling3::GameState::Stones const& alternative, bool finish);
    void draw(QPainter & painter, qreal scale, ShotLog * shot_log, float sheet_width) const;

private:
    size_t index_;
    digitalcurling3::GameState::Stones stones_;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SHEET_HPP
