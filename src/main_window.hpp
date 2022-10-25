#ifndef DIGITALCURLING3_GUI_MAIN_WINDOW_HPP
#define DIGITALCURLING3_GUI_MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QIcon>

#include "model.hpp"
#include "score_table_model.hpp"
#include "stone_item_delegate.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace digitalcurling3_gui {


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateState(Model::DirtyFlag const&);

private:
    Ui::MainWindow *ui;
    ScoreTableModel score_table_model_;
    StoneItemDelegate stone_item_delegate_;

    void setFrameControlsEnabled(bool enabled);
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_MAIN_WINDOW_HPP
