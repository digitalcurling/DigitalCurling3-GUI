#ifndef DIGITALCURLING3_GUI_SCORE_TABLE_MODEL_HPP
#define DIGITALCURLING3_GUI_SCORE_TABLE_MODEL_HPP

#include <QAbstractTableModel>
#include "model.hpp"

class QTableView;

namespace digitalcurling3_gui {

class ScoreTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ScoreTableModel(QObject * parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /// \brief 表示を更新する際に呼び出す
    void updateState(Model::DirtyFlag dirty_flag);

    void setTableView(QTableView * view);

private:
    QTableView * view_;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_SCORE_TABLE_MODEL_HPP
