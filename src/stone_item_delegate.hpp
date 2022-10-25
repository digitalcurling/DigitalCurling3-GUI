#ifndef DIGITALCURLING3_GUI_STONE_ITEM_DELEGATE_HPP
#define DIGITALCURLING3_GUI_STONE_ITEM_DELEGATE_HPP

#include <QStyledItemDelegate>

namespace digitalcurling3_gui {

class StoneItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StoneItemDelegate(QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_STONE_ITEM_DELEGATE_HPP
