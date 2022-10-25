#ifndef DIGITALCURLING3_GUI_PREFERENCE_DIALOG_HPP
#define DIGITALCURLING3_GUI_PREFERENCE_DIALOG_HPP

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

namespace digitalcurling3_gui {

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = nullptr);
    ~PreferenceDialog();

private:
    Ui::PreferenceDialog *ui;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_PREFERENCE_DIALOG_HPP
