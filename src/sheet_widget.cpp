#include "sheet_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "sheet.hpp"
#include "model.hpp"

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

SheetWidget::SheetWidget(QWidget *parent)
    : QWidget(parent)
{}

void SheetWidget::paintEvent(QPaintEvent *event)
{
    auto const& m = Model::instance();

    QSizeF const sheet_size(qreal(m.sheetWidth()), kBackBoard1Y - kBackBoard0Y);
    QRectF const painting_area = event->rect();

    // 注意: アスペクト比はwidth/heightだが，シートは90度回転しているので，height/widthで計算している．
    auto const aspect_sheet = sheet_size.height() / sheet_size.width();
    auto const aspect_painting_area = painting_area.width() / painting_area.height();
    qreal sheet_area_width;  // シートの長い方の画面上でのサイズ[pixel]
    if (aspect_sheet > aspect_painting_area) {
        // シートのほうが横長
        sheet_area_width = painting_area.width();
    } else {
        // 描画領域のほうが横長
        sheet_area_width = painting_area.width() * aspect_sheet / aspect_painting_area;
    }

    auto const scale = sheet_area_width / sheet_size.height();

    QTransform const matrix(
                0, scale, 0,
                scale, 0, 0,
                painting_area.width() / 2, painting_area.height() / 2, 1);

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

    // 原点を描画
    // painter.setPen(QPen(QColor(255, 0, 0), qreal(1) / scale));  // 1px幅のペン
    // painter.drawLine(0, 0, 1, 0);
    // painter.setPen(QPen(QColor(0, 255, 0), qreal(1) / scale));  // 1px幅のペン
    // painter.drawLine(0, 0, 0, 1);

    painter.end();
}

} // namespace digitalcurling3_gui
