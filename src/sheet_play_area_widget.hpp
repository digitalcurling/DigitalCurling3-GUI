#ifndef DIGITALCURLING3_GUI_SHEET_PLAY_AREA_WIDGET_HPP
#define DIGITALCURLING3_GUI_SHEET_PLAY_AREA_WIDGET_HPP

// シートのプレイエリア表示

#include <QWidget>

namespace digitalcurling3_gui {

class SheetPlayAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SheetPlayAreaWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SHEET_PLAY_AREA_WIDGET_HPP
