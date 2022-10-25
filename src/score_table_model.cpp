#include "score_table_model.hpp"
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QDebug>

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

namespace {

enum class Item {
    kName,
    kStone,
    kThinkingTime,
    kEnd,
    kExEnd,
    kTotalScore,
    kInvalid
};

inline std::pair<Item, int> GetColumnItem(int column)
{
    auto const& m = Model::instance();
    if (column == 0) {
        return { Item::kName, 0 };
    } else if (column == 1) {
        return { Item::kStone, 0 };
    } else if (column == 2) {
        return { Item::kThinkingTime, 0 };
    } else if (column <= m.maxEnd() + 2) {
        return { Item::kEnd, column - 3 };
    } else if (column == m.maxEnd() + 3) {
        return { Item::kExEnd, 0 };
    } else if (column == m.maxEnd() + 4) {
        return { Item::kTotalScore, 0 };
    }

    return { Item::kInvalid, 0 };
}

inline int GetColumnCount()
{
    auto const& m = Model::instance();
    if (m.isLogOpened()) {
        return 5 + m.maxEnd();
    } else {
        return 0;
    }
}

} // unnamed namespace

ScoreTableModel::ScoreTableModel(QObject * parent)
    : QAbstractTableModel(parent)
    , view_(nullptr)
{}

int ScoreTableModel::rowCount(const QModelIndex &parent) const
{
    return 2;
}

int ScoreTableModel::columnCount(const QModelIndex &parent) const
{
    return GetColumnCount();
}

QVariant ScoreTableModel::data(const QModelIndex &index, int role) const
{
    auto const& m = Model::instance();
    if (!m.isLogOpened()) {
        return QVariant();
    }

    auto [item, end] = GetColumnItem(index.column());

    switch (item) {
    case Item::kName:
        switch (role) {
        case Qt::DisplayRole:
            return m.teamName().at(index.row());
            break;
        case Qt::DecorationRole:
            if (m.hammer() == static_cast<dc::Team>(index.row())) {
                return QIcon(":/images/hammer.svg");
            } else {
                return QIcon(":/images/no_hammer.svg");
            }
            break;
        }
        break;

    case Item::kStone:
        break;

    case Item::kThinkingTime:
        switch (role) {
        case Qt::DisplayRole: {
            assert(index.row() < 2);
            auto const count = m.thinkingTimeRemaining().at(index.row()).count();
            return tr("%1.%2s")
                    .arg(count / 1000)
                    .arg(count % 1000);
            break;
        }
        }
        break;

    case Item::kEnd:
        switch (role) {
        case Qt::DisplayRole: {
            assert(index.row() < 2);
            assert(end < m.maxEnd());
            auto const & score = m.scores().at(index.row()).at(end);
            if (score.has_value()) {
                return score.value();
            } else {
                return QString("-");
            }
            break;
        }

        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter | Qt::AlignVCenter);
            break;

        case Qt::BackgroundRole:
            if (end == m.end()) {
                return QBrush(Qt::lightGray);
            }
            break;
        }
        break;

    case Item::kExEnd:
        switch (role) {
        case Qt::DisplayRole: {
            assert(index.row() < 2);
            auto const & score = m.extraEndScore().at(index.row());
            if (score.has_value()) {
                return score.value();
            } else {
                return QString("-");
            }
            break;
        }
        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter | Qt::AlignVCenter);
            break;
        }
        break;

    case Item::kTotalScore:
        switch (role) {
        case Qt::DisplayRole:
            assert(index.row() < 2);
            return m.totalScore().at(index.row());
            break;

        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter | Qt::AlignVCenter);
            break;
        }
        break;

    case Item::kInvalid:
        break;
    }
    return QVariant();
}

QVariant ScoreTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    auto const& m = Model::instance();
    if (!m.isLogOpened()) return QVariant();

    switch (orientation) {
    case Qt::Horizontal: {
        auto [item, end] = GetColumnItem(section);

        switch (item) {
        case Item::kName:
            switch (role) {
            case Qt::DisplayRole:
                return tr("Team");
                break;
            }
            break;

        case Item::kStone:
            switch (role) {
            case Qt::DisplayRole:
                return tr("Stones");
                break;
            }
            break;

        case Item::kThinkingTime:
            switch (role) {
            case Qt::DisplayRole:
                return tr("Time");
                break;
            }
            break;

        case Item::kEnd:
            switch (role) {
            case Qt::DisplayRole:
                return end;
                break;
            }
            break;

        case Item::kExEnd:
            switch (role) {
            case Qt::DisplayRole:
                return tr("Ex");
                break;
            }
            break;

        case Item::kTotalScore:
            switch (role) {
            case Qt::DisplayRole:
                return tr("Total");
                break;
            }
            break;

        case Item::kInvalid:
            break;
        }
        break;
    }

    case Qt::Vertical:
        switch (role) {
        case Qt::DisplayRole:
            return section;
            break;
        }
        break;
    }

    // if (orientation == Qt::Horizontal) {
    //     if (section == 0) {
    //         if (role == Qt::DisplayRole) {
    //             return QString("Name");
    //         }
    //     } else if (section == 1) {
    //         if (role == Qt::DisplayRole) {
    //             return QString("Stones");
    //         }
    //     } else if (section <= m.maxEnd() + 1) {
    //         auto const end = section - 2;
    //         if (role == Qt::DisplayRole) {
    //             return end;  // end
    //         }
    //         // headerの色は変更できない模様
    //     } else if (section == m.maxEnd() + 2) {
    //         if (role == Qt::DisplayRole) {
    //             return QString("Ex");
    //         }
    //     } else if (section == m.maxEnd() + 3) {
    //         if (role == Qt::DisplayRole) {
    //             return QString("Total");
    //         }
    //     }

    // } else if (orientation == Qt::Vertical) {
    //     if (role == Qt::DisplayRole) {
    //         return section;
    //     }
    // }

    return QVariant();
}

void ScoreTableModel::updateState(Model::DirtyFlag dirty_flag)
{
    auto const& m = Model::instance();

    if (dirty_flag.game_log) {
        emit headerDataChanged(Qt::Vertical, 0, 1);
        if (view_) {
            // アイテムの大きさを調整
            view_->resizeColumnsToContents();
            view_->resizeRowsToContents();

            // 表全体の高さを調整
            {
                int const header_height = view_->horizontalHeader()->height();
                int const scroll_bar_height = view_->horizontalScrollBar()->height();

                int const count = view_->verticalHeader()->count();
                int row_total_height = 0;
                for (int i = 0; i < count; ++i) {
                    if (!view_->verticalHeader()->isSectionHidden(i)) {
                        row_total_height += view_->verticalHeader()->sectionSize(i);
                    }
                }

                view_->setMaximumHeight(2 + header_height + scroll_bar_height + row_total_height);
            }
        }
    }

    if (dirty_flag.turn) {
        if (m.isLogOpened()) {
            auto const column_count = GetColumnCount();
            if (column_count > 0) {
                emit dataChanged(index(0, 0), index(1, column_count - 1)); // 全部更新
            }
        }

    }
}

void ScoreTableModel::setTableView(QTableView * view)
{
    view_ = view;
}

} // namespace digitalcurling3_gui
