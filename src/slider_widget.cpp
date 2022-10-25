#include "slider_widget.hpp"

#include <QDebug>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainter>

namespace digitalcurling3_gui {

namespace {
    constexpr int kBarMarginX = 5;
    constexpr int kBarHeight = 4;
    constexpr int kCursorWidth = 10;
} // unnamed namespace

SliderWidget::SliderWidget(QWidget *parent)
    : QWidget(parent)
    , value_(0)
    , min_value_(0)
    , max_value_(100)
{
    //setMouseTracking(true); // これは要らない．
}

void SliderWidget::setValue(int new_value)
{
    value_ = qBound(min_value_, new_value, max_value_);
    update();
}

void SliderWidget::setRange(int min, int max)
{
    min_value_ = std::min(min, max);
    max_value_ = max;
    setValue(value_);
}

void SliderWidget::paintEvent(QPaintEvent *event)
{
    // TODO switch color by enabled/disabled

    // qDebug() << "MySlider::paintEvent";

    QPainter painter;
    painter.begin(this);

    auto const rect = event->rect();

    // painter.setPen(Qt::NoPen);
    // painter.fillRect(rect, QColor(255,255,255)); // TODO: debug

    auto const center = rect.center();
    auto const [bar_min_x, bar_max_x] = barMinMaxX();
    auto const cursor_x = cursorX();

    QRect const bar_rect(bar_min_x, center.y() - kBarHeight / 2, bar_max_x - bar_min_x, kBarHeight);
    if (isEnabled()) {
        painter.fillRect(bar_rect, QColor(200, 200, 200));
        QRect const played_rect(bar_rect.x(), bar_rect.y(), cursor_x - bar_min_x, bar_rect.height());
        painter.fillRect(played_rect, QColor(255, 255, 255));
    }


    if (isEnabled()) {
        painter.setPen(QPen(QColor(150,150,150), 1));
    } else {
        painter.setPen(QPen(QColor(220,220,220), 1));
    }
    painter.drawRect(bar_rect);

    if (isEnabled()) {
        QRect const cursor_rect(cursor_x - kCursorWidth / 2, 0, kCursorWidth, rect.height());  // TODO
        painter.fillRect(cursor_rect, QColor(0, 0, 0));
    }

    painter.end();
}

void SliderWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto const pos = event->position().toPoint();
    // qDebug() << "MySlider::mouseMoveEvent" << pos;
    if (selecting_) {
        readValue(pos);
    }
}

void SliderWidget::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "MySlider::mousePressEvent";
    selecting_ = true;
    auto const pos = event->position().toPoint();
    readValue(pos);
}

void SliderWidget::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    // qDebug() << "MySlider::mouseReleaseEvent";
    selecting_ = false;
}

void SliderWidget::readValue(const QPoint &mouse_position)
{
    // TODO read value from mouse_position
    int clamped_mpos_x = qBound(0, mouse_position.x(), size().width());  // マウス位置を領域内にクランプ

    auto const [bar_min_x, bar_max_x] = barMinMaxX();
    auto const bar_width = std::max(1, bar_max_x - bar_min_x);
    double const ratio = double(clamped_mpos_x - bar_min_x) / double(bar_width);

    auto const new_value = int(std::round(ratio * (max_value_ - min_value_) + min_value_));

    setValue(new_value);
    emit actionTriggered();
}

std::pair<int, int> SliderWidget::barMinMaxX() const
{
    int min = kBarMarginX;
    int max = std::max(min, size().width() - kBarMarginX);
    return { min, max };
}

int SliderWidget::cursorX() const
{
    auto const [bar_min_x, bar_max_x] = barMinMaxX();
    auto const value_width = std::max(max_value_ - min_value_, 1);
    double const ratio = double(value_ - min_value_) / double(value_width);
    return int(std::round(ratio * (bar_max_x - bar_min_x) + bar_min_x));
}

} // namespace digitalcurling3_gui
