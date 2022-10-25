#include "stone_item_delegate.hpp"
#include "model.hpp"
#include "sheet.hpp"
#include <QPainter>

namespace digitalcurling3_gui {

namespace {

constexpr int kStoneRadius = 5;
constexpr int kMargin = 1;
constexpr int kStoneBoxSize = (kStoneRadius + kMargin) * 2;

} // unnamed namespace


StoneItemDelegate::StoneItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{

}
void StoneItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    auto const& m = Model::instance();
    if (!m.isLogOpened()) return;

    assert(index.row() < 2);
    painter->setPen(Qt::NoPen);

    auto const stone_count = m.turnsPerEnd() / 2;
    auto const& stone_info = m.stonesInfo().at(index.row());
    for (int i = 0; i < stone_count; ++i) {
        auto const si = stone_info.at(i);

        if (si == Model::StoneInfo::kInShot) {
            QRect const rect(option.rect.left() + kStoneBoxSize * i,
                             option.rect.top(),
                             kStoneBoxSize,
                             option.rect.height());
            painter->setBrush(Qt::lightGray);
            painter->drawRect(rect);
        }

        if (si == Model::StoneInfo::kBeforeShot
                || si == Model::StoneInfo::kInShot) {

            QRect const rect(option.rect.left() + kStoneBoxSize * i + kMargin,
                             option.rect.center().y() - kStoneRadius,
                             kStoneRadius * 2,
                             kStoneRadius * 2);
            painter->setBrush(kTeamColor.at(index.row()));
            painter->drawEllipse(rect);
        }
    }
}

QSize StoneItemDelegate::sizeHint(const QStyleOptionViewItem &option,
               const QModelIndex &index) const
{
    auto const& m = Model::instance();
    auto const stone_count = m.isLogOpened() ? (m.turnsPerEnd() / 2) : 1;
    return QSize(kStoneBoxSize * stone_count, kStoneBoxSize);
}

} // namespace digitalcurling3_gui
