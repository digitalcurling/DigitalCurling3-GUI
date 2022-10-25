#ifndef DIGITALCURLING3_GUI_MODEL_HPP
#define DIGITALCURLING3_GUI_MODEL_HPP

#include <memory>
#include <deque>
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

#include "game_log.hpp"
#include "shot_log.hpp"
#include "settings.hpp"
#include "sheet.hpp"

class QPainter;


namespace digitalcurling3_gui {

class Model : public QObject
{
    Q_OBJECT

public:


    // struct ---

    struct DirtyFlag {
        bool game_log : 1;
        bool turn : 1;
        bool animation : 1;
        DirtyFlag();
        void setAll(bool flag);
    };

    enum class StoneInfo {
        kBeforeShot,
        kInShot,
        kAfterShot
    };

    enum class AnimInfo {
        kInvalid, // 再生無効
        kPlay,    // プレイ中
        kPause,   // 一時停止中 (最終状態でない)
        kFinish   // 最終状態で停止中
    };


    // アニメーションのフレームごとに変化する状態
    struct AnimState {
    };


    // signleton ---

    /// \brief シングルトンインスタンスが存在するかを得る
    /// \return シングルトンインスタンスが存在するなら \c true
    static bool hasInstance() { return instance_ != nullptr; }

    /// \brief シングルトンインスタンスを取得する
    /// \return シングルトンインスタンス
    static Model & instance() { assert(hasInstance()); return *instance_; }


    // constructor/destructor ---

    explicit Model(QObject *parent = nullptr);

    virtual ~Model();
    Model(Model const&) = delete;
    Model & operator = (Model const&) = delete;


    // getter ---

    /// \return ログファイルが開かれているなら true
    bool isLogOpened() const;


    // --- game_log ---
    QString const& fileName() const;
    QString const& protocolVersion() const;
    std::array<QString, 2> const& teamName() const;
    QString const& gameId() const;
    QString const& dateTime() const;
    int maxEnd() const;
    float sheetWidth() const;
    int turnsPerEnd() const;
    std::optional<QString> simulatorType() const;
    std::optional<digitalcurling3::GameSetting> gameSetting() const;


    // --- turn ---
    int end() const;
    int shot() const; // 0 <= shot < dc::GameState::kShotsPerEnd
    bool isEndStart() const;
    digitalcurling3::Team hammer() const;
    int totalShotFrame() const; // アニメーション全体のフレーム数
    float totalShotTime() const;  // アニメーション全体の秒数
    bool isFirstTurn() const; // 最初のターンなら true
    bool isFinalTurn() const; // 最後のターンなら true
    bool isGameOver() const;  // 最後のターンかつアニメーション終了
    decltype(digitalcurling3::GameState::scores) const& scores() const;  // 現在のスコア
    decltype(digitalcurling3::GameState::extra_end_score) const& extraEndScore() const;  // 現在の延長エンドスコア
    std::array<int, 2> totalScore() const; // 現在のト合計スコア
    std::optional<digitalcurling3::GameResult> const& gameResult() const;  // 試合結果
    AnimInfo animInfo() const;
    std::array<std::chrono::milliseconds, 2> const& thinkingTimeRemaining() const;
    std::array<std::vector<StoneInfo>, 2> const& stonesInfo() const;
    std::optional<GameLog::MoveResult> moveResult() const;
    std::vector<digitalcurling3::GameState::Stones> const* shotLogFrames() const;
    float shotLogSecondsPerFrame() const;

    // --- animation ---
    int currentShotFrame() const; // 現在のフレーム
    float currentShotTime() const;  // 現在の経過秒数


    void drawSheet(QPainter & painter, qreal scale) const;


public slots: // ---

    void openLog(QString const& file_name);
    void closeLog();

    /// \brief 指定したフレームに行く + シグナル stateUpdated() が発生する
    /// \param frame 行くフレーム．全フレーム数よりも大きいと finish 状態になる．
    void setFrame(int frame);

    void play();
    void pause();
    void goNextTurn();
    void goPreviousTurn();
    void goNextEnd();
    void goPreviousEnd();

    void reloadSettings();

signals: // ---

    void errorOccured(QString const& error_message);  // 何らかのエラーメッセージを出す

    void stateUpdated(digitalcurling3_gui::Model::DirtyFlag const& flag);


private:  // ---

    // singleton
    inline static Model * instance_ = nullptr;

    //
    Settings settings_;


    DirtyFlag dirty_flag_;

    // --- game_log ---
    std::unique_ptr<GameLog> game_log_;
    QString file_name_;
    std::filesystem::path file_dir_;
    QString protocol_version_;
    std::array<QString, 2> team_name_;
    QString game_id_;
    QString date_time_;

    // --- turn ---
    int end_;
    int shot_; // 0 <= shot < dc::GameState::kShotsPerEnd
    bool end_start_;
    std::unique_ptr<ShotLog> shot_log_;
    std::array<std::vector<StoneInfo>, 2> stones_info_;
    digitalcurling3::GameState::Stones game_state_stones_;

    // --- animation ---
    int current_shot_frame_; // 現在のフレーム
    float current_shot_time_;  // 現在の経過秒数
    SheetDrawer sheet_drawer_;


    // アニメーション再生用
    QTimer animation_timer_;
    QElapsedTimer elapsed_timer_;
    int animation_count_; // TODO 使ってる?
    int animation_started_frame_;

    /// \brief 表示状態の更新を通知する
    ///
    /// 通知後は dirty_flag_ がリセットされる
    void emitStateUpdated();

    void resetAll();

    void startAnimationCore();  // アニメーション再生
    void stopAnimationCore();  // アニメーションの一時停止

    /// \brief 指定したフレームに行く．シグナル animationStateUpdated() は発生しない
    /// \param frame 行くフレーム．全フレーム数よりも大きいと finish 状態になる．
    void setShotFrameCore(int frame);

    /// \brief 指定のターンのフレーム 0 に飛ぶ．
    ///
    /// end, shot, end_startは適切な値でないといけない．
    ///
    ///
    void setTurnCore(int end, int shot, bool end_start, bool force_reload = false);

    digitalcurling3::GameState const& showingGameStateForScore() const;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_MODEL_HPP
