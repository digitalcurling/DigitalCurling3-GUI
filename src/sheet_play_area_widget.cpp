#include "sheet_play_area_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "sheet.hpp"
#include "model.hpp"

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

SheetPlayAreaWidget::SheetPlayAreaWidget(QWidget *parent)
    : QWidget(parent)
{}

void SheetPlayAreaWidget::paintEvent(QPaintEvent *event)
{
    constexpr auto kTrimTopY = kBackLine1Y + 0.5;
    constexpr auto kPrefTrimBottomY = kHogLine1Y - 0.5;
    constexpr auto kPrefTrimHeight = kTrimTopY - kPrefTrimBottomY;

    auto const& m = Model::instance();

    auto const sheet_width = qreal(m.sheetWidth());
    QRectF const painting_area = event->rect();

    auto const aspect_pref = sheet_width / kPrefTrimHeight;
    auto const aspect_paint = painting_area.width() / painting_area.height();

    qreal scale;
    if (aspect_pref < aspect_paint) {
        // 横に空白が出来る場合
        scale = painting_area.height() / kPrefTrimHeight;
    } else {
        // 縦長になる場合
        scale = painting_area.width() / sheet_width;
    }

    qreal side_factor = 1.0;
    if (m.isLogOpened()
            && dc::coordinate::GetShotSide(std::uint8_t(m.end())) == dc::coordinate::Id::kShot1) {
        side_factor = -1.0;
    }

    QTransform const matrix(
                scale * side_factor, 0, 0,
                0, -scale * side_factor, 0,
                painting_area.width() / 2, kTrimTopY * scale, 1);

    // 描画

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景色
    painter.fillRect(painting_area, QBrush(kSheetBackgroundColor));

    // 座標変換の設定
    painter.setWorldMatrixEnabled(true);
    painter.setWorldTransform(matrix);

    // シートの描画
    m.drawSheet(painter, scale);

    painter.end();
}

} // namespace digitalcurling3_gui
