#include "main_window.hpp"
#include "./ui_main_window.h"

#include <QtCore/QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringBuilder>
#include <QtWidgets>
#include <QtWidgets>

#include "digitalcurling3/digitalcurling3.hpp"

#include "version.hpp"
#include "game_log.hpp"
#include "settings.hpp"
#include "preference_dialog.hpp"

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , score_table_model_()
{
    ui->setupUi(this);

    // window icon ---

    setWindowIcon(QIcon(":/images/logo.svg"));


    // input ---

    connect(ui->actionOpenLog, &QAction::triggered, this, [this] {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Log File"), QString(), "Log File (*.dcl2)");

        if (!fileName.isEmpty()) {
            Model::instance().openLog(fileName);
        }
    });

    connect(ui->actionCloseLog, &QAction::triggered, &Model::instance(), &Model::closeLog);
    ui->actionCloseLog->setEnabled(false);  // 初期状態でcloseは選択不可．

    connect(ui->actionAbout, &QAction::triggered, this, [this] {
        QString message = QString("<h1>DigitalCurling3 GUI</h1>"
                                  "<p>Version: %1<br/>"
                                  "Commit: %2<br/>"
                                  "Date: %3<br/>"
                                  "Digital Curling: %4<br/>"
                                  "Qt: %5</p>")
          .arg(QString::fromStdString(GetVersion()),
               QString::fromStdString(GetGitSHA1()),
               QString::fromStdString(GetGitDate()),
               QString::fromStdString(dc::GetVersion()),
               QString(qVersion()));
        QMessageBox::information(this, "Digital Curling GUI", message);
    });

    connect(ui->actionPreference, &QAction::triggered, this, [this] {
        Model::instance().pause();
        PreferenceDialog dialog(this);
        connect(&dialog, &PreferenceDialog::accepted, &Model::instance(), &Model::reloadSettings);
        dialog.exec();
    });

    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);

    ui->actionPlay->setIcon(QIcon(":/images/play.svg"));
    ui->actionPause->setIcon(QIcon(":/images/pause.svg"));
    ui->actionNextTurn->setIcon(QIcon(":/images/next_turn.svg"));
    ui->actionPreviousTurn->setIcon(QIcon(":/images/prev_turn.svg"));
    ui->actionNextEnd->setIcon(QIcon(":/images/next_end.svg"));
    ui->actionPreviousEnd->setIcon(QIcon(":/images/prev_end.svg"));
    connect(ui->actionPlay, &QAction::triggered, &Model::instance(), &Model::play);
    connect(ui->actionPause, &QAction::triggered, &Model::instance(), &Model::pause);
    connect(ui->actionNextTurn, &QAction::triggered, &Model::instance(), &Model::goNextTurn);
    connect(ui->actionPreviousTurn, &QAction::triggered, &Model::instance(), &Model::goPreviousTurn);
    connect(ui->actionNextEnd, &QAction::triggered, &Model::instance(), &Model::goNextEnd);
    connect(ui->actionPreviousEnd, &QAction::triggered, &Model::instance(), &Model::goPreviousEnd);
    ui->playButton->setDefaultAction(ui->actionPlay);
    ui->pauseButton->setDefaultAction(ui->actionPause);
    ui->nextTurnButton->setDefaultAction(ui->actionNextTurn);
    ui->previousTurnButton->setDefaultAction(ui->actionPreviousTurn);
    ui->nextEndButton->setDefaultAction(ui->actionNextEnd);
    ui->previousEndButton->setDefaultAction(ui->actionPreviousEnd);

    connect(ui->frameSlider, &SliderWidget::actionTriggered, &Model::instance(), [frameSlider = ui->frameSlider] {
        auto & m = Model::instance();

        // pause() の呼び出しで frameSliderの値が変わってしまうので，一度保存して再度設定している．
        auto const frame = frameSlider->value();
        m.pause();
        m.setFrame(frame);
    });


    // output ---

    connect(&Model::instance(), &Model::stateUpdated, this, &MainWindow::updateState);
    connect(&Model::instance(), &Model::errorOccured, this, [this](QString const& message) {
        statusBar()->showMessage(message);
    });


    // score table ---

    ui->scoreTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->scoreTableView->setFocusPolicy(Qt::NoFocus);
    ui->scoreTableView->setSelectionMode(QAbstractItemView::NoSelection);
    // ui->scoreTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // リサイズ禁止
    ui->scoreTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);   // リサイズ禁止
    ui->scoreTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 水平スクロールバーは常に表示する
    ui->scoreTableView->setModel(&score_table_model_);
    score_table_model_.setTableView(ui->scoreTableView);
    ui->scoreTableView->setItemDelegateForColumn(1, &stone_item_delegate_);


    // initialize ---

    Model::DirtyFlag flag;
    flag.setAll(true);
    updateState(flag);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateState(Model::DirtyFlag const& dirty_flag)
{
    auto const& m = Model::instance();

    //
    if (dirty_flag.game_log) {
        // Window title
        if (m.isLogOpened()) {
            setWindowTitle(QString("DigitalCurling3 GUI [") % m.fileName() % "]");
        } else {
            setWindowTitle("DigitalCurling3 GUI");
        }

        // 閉じるボタン
        ui->actionCloseLog->setEnabled(m.isLogOpened());

        // 試合設定
        {
            QString text("<ul>");
            auto const game_setting = m.gameSetting();

            QString text_sheet_width;
            QString text_five_rock_rule;

            if (game_setting) {
                text_sheet_width = QString("%1m").arg(game_setting->sheet_width);
                text_five_rock_rule = game_setting->five_rock_rule ? "true" : "false";
            }

            text += "<li>" % tr("Game ID") % ": " % m.gameId() %
                    "</li><li>" % tr("Date Time") % ": " % m.dateTime() %
                    "</li><li>" % tr("Sheet Width") % ": " % text_sheet_width %
                    "</li><li>" % tr("Five rock rule") % ": " % text_five_rock_rule %
                    "</li><li>" % tr("Simulator") % ": " % m.simulatorType().value_or(QString()) %
                    "</li>";

            text += "</ul>";

            ui->gameSettingLabel->setText(text);
        }
    }

    //
    if (dirty_flag.turn) {
        if (m.isLogOpened()) {
            ui->actionPause->setEnabled(m.animInfo() == Model::AnimInfo::kPlay);
            ui->actionNextTurn->setEnabled(!m.isFinalTurn());
            ui->actionPreviousTurn->setEnabled(!m.isFirstTurn());
            ui->actionNextEnd->setEnabled(!m.isFinalTurn());
            ui->actionPreviousEnd->setEnabled(!m.isFirstTurn());
        } else {
            // disable controls
            ui->actionPlay->setEnabled(false);
            ui->actionPause->setEnabled(false);
            ui->actionNextTurn->setEnabled(false);
            ui->actionPreviousTurn->setEnabled(false);
            ui->actionNextEnd->setEnabled(false);
            ui->actionPreviousEnd->setEnabled(false);
            setFrameControlsEnabled(false);
        }

        // 再生ボタン
        if (!m.isLogOpened() || m.isGameOver()) {
            // ゲーム終了
            ui->actionPlay->setEnabled(false);
        } else {
            ui->actionPlay->setEnabled(true);
        }

        // エンドとショットの表示
        {
            QString text;
            if (m.isLogOpened()) {
                text = tr("End %1, ").arg(m.end());

                if (m.isEndStart()) {
                    text += tr("Start");
                } else {
                    text += tr("Shot %1").arg(m.shot());
                }

                // 試合結果のテキスト
                if (m.gameResult().has_value()) {
                    text += " -> <b>";
                    QString text_winner_name;
                    QString text_winner_num;
                    dc::Team const winner = m.gameResult()->winner;

                    if (winner != dc::Team::kInvalid) {
                        text_winner_name = m.teamName().at(static_cast<size_t>(winner));
                        text_winner_num = QString::number(static_cast<int>(winner));
                    }

                    switch (m.gameResult()->reason) {
                    case dc::GameResult::Reason::kScore:
                        text += tr("Team%1 \"%2\" Won by Score!").arg(text_winner_num, text_winner_name);
                        break;
                    case dc::GameResult::Reason::kConcede:
                        text += tr("Team%1 \"%2\" Won by Concede!").arg(text_winner_num, text_winner_name);
                        break;

                    case dc::GameResult::Reason::kTimeLimit:
                        text += tr("Team%1 \"%2\" Won by Time Limit!").arg(text_winner_num, text_winner_name);
                        break;

                    case dc::GameResult::Reason::kDraw:
                        text += tr("Draw");
                        break;

                    default:
                        assert(false);
                        break;
                    }

                    text += "</b>";
                }

            } else {
                text = tr("Open a Log File");
            }

            ui->endShotLabel->setText(text);
        }

        // Move result
        static auto const get_move_text = [](dc::Move const& move) -> QString
        {
            if (std::holds_alternative<dc::moves::Shot>(move)) {
                auto const & shot = std::get<dc::moves::Shot>(move);
                return QString("Shot (vx: %1, vy: %2, rotate: %3)")
                        .arg(shot.velocity.x)
                        .arg(shot.velocity.y)
                        .arg(shot.rotation == dc::moves::Shot::Rotation::kCCW ? "ccw" : "cw");
            } else if (std::holds_alternative<dc::moves::Concede>(move)) {
                return QString("Concede");
            } else {
                std::runtime_error("unexpected move type");
            }
            return QString();
        };

        QString text_selected_move;
        QString text_actual_move;
        QString text_fgz;

        auto const move_result = m.moveResult();
        if (move_result) {
            text_selected_move = get_move_text(move_result->input_move);
            text_actual_move = get_move_text(move_result->actual_move);
            text_fgz = move_result->free_guard_zone_foul ? tr("Applied") : tr("Not Applied");
        }

        QString text = "<ul><li>" % tr("Selected move") % ": " % text_selected_move
                % "</li><li>" % tr("Actual move") % ": " % text_actual_move
                % "</li><li>" % tr("Free guard zone rule") % ": " % text_fgz
                % "</li></ul>";

        ui->moveResultLabel->setText(text);
    }

    //
    if (dirty_flag.animation) {

        if (m.isLogOpened() && m.animInfo() != Model::AnimInfo::kInvalid) {

            setFrameControlsEnabled(true);

            // update sheet
            if (m.animInfo() == Model::AnimInfo::kPlay) {
                ui->sheet1->repaint(); // immediate redraw
                ui->sheet2->repaint(); // immediate redraw
            } else {
                ui->sheet1->update();
                ui->sheet2->update();
            }

            if (m.animInfo() == Model::AnimInfo::kFinish) {
                ui->currentTimeLabel->setText(tr("finish"));
            } else {
                ui->currentTimeLabel->setText(tr("%1s").arg(QString::number(m.currentShotTime(), 'f', 1)));
            }
            ui->totalTimeLabel->setText(tr("%1s").arg(QString::number(m.totalShotTime(), 'f', 1)));

            ui->frameSlider->setRange(0, m.totalShotFrame());
            ui->frameSlider->setValue(m.currentShotFrame());

        } else {
            setFrameControlsEnabled(false);

            ui->sheet1->update();
            ui->sheet2->update();

            ui->currentTimeLabel->setText("--");
            ui->totalTimeLabel->setText("--");
            ui->frameSlider->setRange(0, 0);
            ui->frameSlider->setValue(0);
        }
    }

    score_table_model_.updateState(dirty_flag);

    // debug
    // {
    //     QString text("anim_info: ");
    //     if (m.isLogOpened()) {
    //         switch (m.animInfo()) {
    //             case Model::AnimInfo::kInvalid: text += "invalid"; break;
    //             case Model::AnimInfo::kPlay:    text += "play";    break;
    //             case Model::AnimInfo::kPause:   text += "pause";   break;
    //             case Model::AnimInfo::kFinish:  text += "finish";  break;
    //         }
    //     } else {
    //         text += "not opened";
    //     }
    //     ui->label->setText(text);
    // }
}

void MainWindow::setFrameControlsEnabled(bool enabled)
{
    ui->frameSlider->setEnabled(enabled);
    ui->currentTimeLabel->setEnabled(enabled);
    ui->totalTimeLabel->setEnabled(enabled);
}

} // namespace digitalcurling3_gui
