#ifndef DIGITALCURLING3_GUI_SHEET_WIDGET_HPP
#define DIGITALCURLING3_GUI_SHEET_WIDGET_HPP

// シート全体表示

#include <QWidget>

namespace digitalcurling3_gui {

class SheetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SheetWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SHEET_WIDGET_HPP
