#ifndef DIGITALCURLING3_GUI_SLIDER_WIDGET_HPP
#define DIGITALCURLING3_GUI_SLIDER_WIDGET_HPP

#include <QWidget>
#include <QPoint>

namespace digitalcurling3_gui {

class SliderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SliderWidget(QWidget *parent = nullptr);

    int value() const { return value_; }

public slots:
    void setValue(int);
    void setRange(int min, int max);

signals:
    // ユーザーが操作したときに発火する．setValueの直接呼出しでは発火しない．
    void actionTriggered();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int value_;
    int min_value_;
    int max_value_;
    bool selecting_;

    void readValue(QPoint const& mouse_position);
    std::pair<int, int> barMinMaxX() const;
    int cursorX() const;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SLIDER_WIDGET_HPP
