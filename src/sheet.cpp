#include "sheet.hpp"

#include <algorithm>
#include "digitalcurling3/digitalcurling3.hpp"
#include "shot_log.hpp"

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

namespace {

void DrawCircle(QPainter & painter, QPointF const& center, qreal radius, QColor const& color)
{
    painter.setBrush(color);
    QRectF const rect(center.x() - radius, center.y() - radius,
                radius * 2.0, radius * 2.0);
    painter.drawEllipse(rect);
}

} // unnamed namespace

SheetDrawer::SheetDrawer()
    : index_(0)
    , stones_()
{}

void SheetDrawer::reset()
{
    index_ = 0;
    for (auto & team_stones : stones_) {
        for (auto & stones : team_stones) {
            stones.reset();
        }
    }
}

void SheetDrawer::prepare(ShotLog const* shot_log, float current_time, digitalcurling3::GameState::Stones const& alternative, bool finish)
{
    if (!shot_log) {
        index_ = 0;
        stones_ = alternative;
        return;
    }
    auto const seconds_per_frame = shot_log->secondsPerFrame();
    auto const& frames = shot_log->frames();

    auto const mod = std::fmod(current_time, seconds_per_frame);

    // 負の数になる可能性があるため，負も含む整数としている．
    auto const idx_temp = static_cast<std::int64_t>(std::round((current_time - mod) / seconds_per_frame));
    index_ = static_cast<size_t>(std::max(static_cast<decltype(idx_temp)>(0), idx_temp));

    assert(frames.size() > 0);
    if (finish) {
        stones_ = shot_log->stonesAtFinish();
        index_ = frames.size() - 1;
    } else if (index_ + 1 >= frames.size()) {
        stones_ = frames.back();
        index_ = frames.size() - 1;
    } else {
        // 線形補間
        // stones_ret = stones0 * a + stones1 * (1 - a)
        auto const a = mod / seconds_per_frame;
        dc::GameState::Stones const& stones0 = frames.at(index_);
        dc::GameState::Stones const& stones1 = frames.at(index_ + 1);

        for (size_t i_team = 0; i_team < stones_.size(); ++i_team) {
            for (size_t i_stone = 0; i_stone < stones_[i_team].size(); ++i_stone) {
                auto const& stone0 = stones0[i_team][i_stone];
                auto const& stone1 = stones1[i_team][i_stone];
                auto & stone = stones_[i_team][i_stone];

                if (!stone0 || !stone1) {
                    stone = std::nullopt;
                } else {
                    stone.emplace();
                    stone->position = stone0->position * (1.f - a) + stone1->position * a;
                    stone->angle = stone0->angle * (1.f - a) + stone1->angle * a;
                }
            }
        }
    }
}

void SheetDrawer::draw(QPainter & painter, qreal scale, ShotLog * shot_log, float sheet_width) const
{
    QSizeF const sheet_size(qreal(sheet_width), kBackBoard1Y - kBackBoard0Y);
    // シートを描画
    QRectF const sheet_rect(-sheet_size.width() / qreal(2), -sheet_size.height() / qreal(2),
                      sheet_size.width(), sheet_size.height());
    painter.fillRect(sheet_rect, kSheetColor);

    // ハウスを描画
    painter.setPen(Qt::NoPen);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine0Y),  1.829, kOuterCircleColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine0Y),  1.219, kSheetColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine0Y),  0.610, kInnerCircleColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine0Y),  0.152, kSheetColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine1Y), 1.829, kOuterCircleColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine1Y), 1.219, kSheetColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine1Y), 0.610, kInnerCircleColor);
    DrawCircle(painter, QPointF(kCenterLineX, kTeeLine1Y), 0.152, kSheetColor);

    painter.setPen(QPen(kLineColor, qreal(1) / scale));
    // センターライン
    painter.drawLine(QPointF(kCenterLineX, kBackBoard0Y), QPointF(kCenterLineX, kBackBoard1Y));
    // ホグライン
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kHogLine0Y), QPointF(sheet_size.width() / 2.0, kHogLine0Y));
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kHogLine1Y), QPointF(sheet_size.width() / 2.0, kHogLine1Y));
    // ティーライン
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kTeeLine0Y), QPointF(sheet_size.width() / 2.0, kTeeLine0Y));
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kTeeLine1Y), QPointF(sheet_size.width() / 2.0, kTeeLine1Y));
    // バックライン
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kBackLine0Y), QPointF(sheet_size.width() / 2.0, kBackLine0Y));
    painter.drawLine(QPointF(-sheet_size.width() / 2.0, kBackLine1Y), QPointF(sheet_size.width() / 2.0, kBackLine1Y));
    // ハック
    painter.drawLine(QPointF(-0.5, kHack0Y), QPointF(0.5, kHack0Y));
    painter.drawLine(QPointF(-0.5, kHack1Y), QPointF(0.5, kHack1Y));


    // ストーンを描画 ---
    auto constexpr stone_radius = dc::ISimulator::kStoneRadius;
    auto constexpr cover_radius = qreal(0.1065);

    QPen angle_pen(QColor(2,2,2), 2.0 / scale);

    for (size_t i_team = 0; i_team < stones_.size(); ++i_team) {
        for (size_t i_stone = 0; i_stone < stones_[i_team].size(); ++i_stone) {
            auto const & stone = stones_[i_team][i_stone];
            if (!stone) continue;

            QPointF const stone_center(stone->position.x, stone->position.y);

            painter.setPen(Qt::NoPen);
            DrawCircle(painter, stone_center, stone_radius, kStoneColor);
            DrawCircle(painter, stone_center, cover_radius, kTeamColor[i_team]);
            painter.setPen(angle_pen);
            QPointF line_end(stone_radius * qreal(std::cos(stone->angle)) + qreal(stone->position.x),
                             stone_radius * qreal(std::sin(stone->angle)) + qreal(stone->position.y));
            painter.drawLine(stone_center, line_end);
        }
    }

    if (!shot_log) return;

    auto const& frames = shot_log->frames();

    auto const draw_line_if_neq = [&painter](std::optional<dc::Transform> const& a, std::optional<dc::Transform> const& b)
    {
        if (!a.has_value() || !b.has_value()) {
            return;
        }
        if (a->position.x == b->position.x &&
                a->position.y == b->position.y) {
            return;
        }
        ;
        painter.drawLine(QPointF(a->position.x, a->position.y),
                         QPointF(b->position.x, b->position.y));
    };

    // 軌跡を表示する ---
    for (size_t i_team = 0; i_team < stones_.size(); ++i_team) {
        QPen traj_pen(kTeamTrajectoryColor[i_team], 2.0 / scale);
        painter.setPen(traj_pen);
        for (size_t i_stone = 0; i_stone < stones_[i_team].size(); ++i_stone) {
            // indexのフレームまで
            for (size_t i_frame = 0; i_frame < index_; ++i_frame) {
                draw_line_if_neq(frames[i_frame][i_team][i_stone],
                                 frames[i_frame + 1][i_team][i_stone]);
            }

            // indexのフレームから現在の位置
            draw_line_if_neq(frames[index_][i_team][i_stone],
                             stones_[i_team][i_stone]);
        }
    }
}

} // namespace digitalcurling3_gui
