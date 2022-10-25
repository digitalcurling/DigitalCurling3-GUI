#include "model.hpp"
#include <sstream>
#include <iomanip>
#include <QStringBuilder>
#include <QSettings>
#include <QDebug>

#include "settings.hpp"
#include "coord.hpp"

namespace dc = digitalcurling3;



namespace digitalcurling3_gui {


namespace {

constexpr int kAnimationInterval = 20;  // milli seconds
constexpr float kAnimationSecondsPerFrame = kAnimationInterval / 1000.0f;

dc::GameState const& GetDefaultGameState()
{
    static dc::GameState const gs;
    return gs;
}

inline size_t GetTurnIndex(int end, int shot)
{
    return static_cast<size_t>(end) * dc::GameState::kShotPerEnd
            + static_cast<size_t>(shot);
}

inline int GetEndFromTurnIndex(size_t turn_index)
{
    return static_cast<int>(turn_index / dc::GameState::kShotPerEnd);
}

inline int GetShotFromTurnIndex(size_t turn_index)
{
    return static_cast<int>(turn_index % dc::GameState::kShotPerEnd);
}

} // unnamed namespace



Model::DirtyFlag::DirtyFlag()
    : game_log(false)
    , turn(false)
    , animation(false)
{}

void Model::DirtyFlag::setAll(bool flag)
{
    game_log = flag;
    turn = flag;
    animation = flag;
}



Model::Model(QObject *parent)
    : QObject(parent)
    , game_log_()
    , animation_timer_(this)
    , elapsed_timer_()
{
    Q_ASSERT(instance_ == nullptr);

    // アニメーションタイマーを設定
    animation_timer_.setInterval(kAnimationInterval);
    connect(&animation_timer_, &QTimer::timeout, this, [this]{
        auto const elapsed = elapsed_timer_.nsecsElapsed();
        double const current_second = static_cast<float>(animation_started_frame_) * kAnimationSecondsPerFrame
                + static_cast<float>(elapsed) / 1.0E9f * settings_.animation_speed;

        setFrame(static_cast<int>(std::round(current_second / kAnimationSecondsPerFrame)));

        if (current_shot_frame_ >= totalShotFrame()) {
            stopAnimationCore();
            emitStateUpdated();
        }
    });

    settings_.load();

    resetAll();

    // シングルトン
    instance_ = this;
}

Model::~Model()
{
    instance_ = nullptr;
}



bool Model::isLogOpened() const
{
    return game_log_ != nullptr;
}

QString const& Model::fileName() const
{
    return file_name_;
}

QString const& Model::protocolVersion() const
{
    return protocol_version_;
}

std::array<QString, 2> const& Model::teamName() const
{
    return team_name_;
}

QString const& Model::gameId() const
{
    return game_id_;
}

QString const& Model::dateTime() const
{
    return date_time_;
}

int Model::maxEnd() const
{
    if (game_log_) {
        return static_cast<int>(game_log_->gameSetting().max_end);
    } else {
        return 0;
    }
}

float Model::sheetWidth() const
{
    if (game_log_) {
        return game_log_->gameSetting().sheet_width;
    } else {
        dc::GameSetting default_game_setting;
        return default_game_setting.sheet_width;
    }
}

int Model::turnsPerEnd() const
{
    if (game_log_) {
        return game_log_->turnsPerEnd();
    } else {
        return 0;
    }
}

std::optional<QString> Model::simulatorType() const
{
    if (game_log_) {
        return QString::fromStdString(game_log_->simulatorType());
    } else {
        return std::nullopt;
    }
}

std::optional<digitalcurling3::GameSetting> Model::gameSetting() const
{
    if (game_log_) {
        return game_log_->gameSetting();
    } else {
        return std::nullopt;
    }
}



int Model::end() const
{
    return end_;
}

int Model::shot() const
{
    return shot_;
}

bool Model::isEndStart() const
{
    return end_start_;
}

digitalcurling3::Team Model::hammer() const
{
    if (game_log_) {
        auto const turn_index = GetTurnIndex(end_, shot_);
        return game_log_->gameStates().at(turn_index).hammer;
    } else {
        return dc::Team::kInvalid;
    }
}

int Model::totalShotFrame() const
{
    return std::ceil(totalShotTime() / kAnimationSecondsPerFrame);
}

float Model::totalShotTime() const
{
    if (shot_log_) {
        return shot_log_->animationLength();
    } else {
        return 0.f;
    }
}

bool Model::isFirstTurn() const
{
    return end_ == 0 && shot_ == 0 && end_start_;
}

bool Model::isFinalTurn() const
{
    if (game_log_) {
        return !end_start_ && GetTurnIndex(end_, shot_) + 2 == game_log_->gameStates().size();
    } else {
        return false;
    }
}

bool Model::isGameOver() const
{
    auto const anim_info = animInfo();
    return isFinalTurn() && (anim_info == AnimInfo::kFinish || anim_info == AnimInfo::kInvalid);
}

decltype(digitalcurling3::GameState::scores) const& Model::scores() const
{
    return showingGameStateForScore().scores;
}

decltype(digitalcurling3::GameState::extra_end_score) const& Model::extraEndScore() const
{
    return showingGameStateForScore().extra_end_score;
}

std::array<int, 2> Model::totalScore() const
{
    auto const& s = showingGameStateForScore();
    return {{
        static_cast<int>(s.GetTotalScore(dc::Team::k0)),
        static_cast<int>(s.GetTotalScore(dc::Team::k1))
    }};
}

std::optional<digitalcurling3::GameResult> const& Model::gameResult() const
{
    return showingGameStateForScore().game_result;
}

Model::AnimInfo Model::animInfo() const
{
    if (!shot_log_ || shot_log_->isAnimationLengthZero()) {
        return AnimInfo::kInvalid;
    }

    if (current_shot_frame_ < totalShotFrame()) {
        if (animation_timer_.isActive()) {
            return AnimInfo::kPlay;
        } else {
            return AnimInfo::kPause;
        }
    } else {
        return AnimInfo::kFinish;
    }
}

std::array<std::chrono::milliseconds, 2> const& Model::thinkingTimeRemaining() const
{
    if (game_log_) {
        // TODO
        auto turn_index = GetTurnIndex(end_, shot_) + 1;
        if (end_start_) {
            --turn_index;  // エンド開始時は前のターンの情報とする (もっとスマートな解決法があればいいけど．．．)
        }
        return game_log_->gameStates().at(turn_index).thinking_time_remaining;
    } else {
        return GetDefaultGameState().thinking_time_remaining;
    }
}

std::array<std::vector<Model::StoneInfo>, 2> const& Model::stonesInfo() const
{
    return stones_info_;
}

std::optional<GameLog::MoveResult> Model::moveResult() const
{
    if (game_log_) {
        if (end_start_) {
            return std::nullopt;
        } else {
            auto const turn_index = GetTurnIndex(end_, shot_);
            return game_log_->moveResults().at(turn_index);
        }
    } else {
        return std::nullopt;
    }
}

std::vector<digitalcurling3::GameState::Stones> const* Model::shotLogFrames() const
{
    if (shot_log_) {
        return &shot_log_->frames();
    } else {
        return nullptr;
    }
}

float Model::shotLogSecondsPerFrame() const
{
    if (shot_log_) {
        return shot_log_->secondsPerFrame();
    } else {
        return 0.f;
    }
}



int Model::currentShotFrame() const
{
    return current_shot_frame_;
}

float Model::currentShotTime() const
{
    return current_shot_time_;
}



void Model::drawSheet(QPainter & painter, qreal scale) const
{
    sheet_drawer_.draw(painter, scale, shot_log_.get(), sheetWidth());
}




void Model::openLog(QString const& file_name)
{
    resetAll();

    try {
        std::filesystem::path file_name_path = file_name.toStdString();
        file_dir_ = file_name_path;
        file_dir_.remove_filename();

        game_log_ = std::make_unique<GameLog>(file_name_path);

        if (game_log_->gameStates().size() <= 1) {
            // この時はエラーとする
            game_log_.reset();

            throw std::runtime_error("game log has no move.");
        }

        // game_log 関連メンバの読み込み
        dirty_flag_.game_log = true;
        file_name_ = file_name;
        protocol_version_ = QString("%1.%2").arg(game_log_->protocolVersionMajor(), game_log_->protocolVersionMinor());
        team_name_[0] = QString::fromStdString(game_log_->teamNames()[0]);
        team_name_[1] = QString::fromStdString(game_log_->teamNames()[1]);
        game_id_ = QString::fromStdString(game_log_->gameId());
        date_time_ = QString::fromStdString(game_log_->dateTime());

        // turn_state_ の読み込み
        dirty_flag_.turn = true;
        for (auto & team_stones_info : stones_info_) {
            team_stones_info.resize(game_log_->turnsPerEnd() / 2, StoneInfo::kBeforeShot);
        }
        setTurnCore(0, 0, true, true);

    } catch (std::exception & e) {
        emit errorOccured("Error on opening game log: " % QString::fromLatin1(e.what()));
    }

    emitStateUpdated();
}

void Model::closeLog()
{
    if (!isLogOpened()) return;

    resetAll();

    emitStateUpdated();
}

void Model::setFrame(int frame)
{
    if (!shot_log_) return;

    setShotFrameCore(frame);

    emitStateUpdated();
}

void Model::play()
{
    if (!isLogOpened()) return;

    switch (animInfo()) {
    case AnimInfo::kPlay:
        // 再生中ならショット終了まで飛ぶ
        stopAnimationCore();
        setShotFrameCore(std::numeric_limits<int>::max());
        emitStateUpdated();
        return;

    case AnimInfo::kPause:
        // 一時停止中なら再生
        startAnimationCore();
        emitStateUpdated();
        return;

    default:
        break;
    }

    // ショットが終了している場合は次のターンに進み，アニメーションを再生する

    stopAnimationCore();

    auto const current_turn_index = GetTurnIndex(end_, shot_);
    int new_end = end_;
    int new_shot = shot_;
    bool new_end_start = end_start_;

    if (end_start_) {
        new_end_start = false;
    } else if (current_turn_index + 2 < game_log_->gameStates().size()) {
        if (shot_ < static_cast<int>(dc::GameState::kShotPerEnd - 1)) {
            ++new_shot;
        } else {
            ++new_end;
            new_shot = 0;
            new_end_start = true;
        }
    } else { // 最終ターンの場合: それ以上することはない
        return;
    }

    setTurnCore(new_end, new_shot, new_end_start);

    if (!end_start_) {
        startAnimationCore();
    }
    emitStateUpdated();
}

void Model::pause()
{
    stopAnimationCore();
    emitStateUpdated();
}

void Model::goNextTurn()
{
    if (!isLogOpened()) return;

    stopAnimationCore();

    auto const anim_info = animInfo();
    if (anim_info == AnimInfo::kPlay || anim_info == AnimInfo::kPause) {
        // ショットが終了してなかったら，現在のショットの最後に行く．
        setShotFrameCore(std::numeric_limits<int>::max());
        emitStateUpdated();
        return;
    }

    auto const current_turn_index = GetTurnIndex(end_, shot_);
    int new_end = end_;
    int new_shot = shot_;
    bool new_end_start = end_start_;

    if (end_start_) {
        new_end_start = false;
    } else if (current_turn_index + 2 < game_log_->gameStates().size()) {
        if (shot_ < static_cast<int>(dc::GameState::kShotPerEnd - 1)) {
            ++new_shot;
        } else {
            ++new_end;
            new_shot = 0;
            new_end_start = true;
        }
    }

    setTurnCore(new_end, new_shot, new_end_start);
    setShotFrameCore(std::numeric_limits<int>::max());
    emitStateUpdated();
}

void Model::goPreviousTurn()
{
    if (!isLogOpened()) return;

    stopAnimationCore();

    auto const current_turn_index = GetTurnIndex(end_, shot_);
    int new_end = end_;
    int new_shot = shot_;
    bool new_end_start = end_start_;

    if (shot_ == 0) {
        if (end_start_) {
            if (current_turn_index > 0) {
                --new_end;
                new_shot = dc::GameState::kShotPerEnd - 1;
                new_end_start = false;
            }
        } else {
            new_end_start = true;
        }
    } else {
        --new_shot;
    }

    setTurnCore(new_end, new_shot, new_end_start);
    setShotFrameCore(std::numeric_limits<int>::max());
    emitStateUpdated();
}

void Model::goNextEnd()
{
    if (!isLogOpened()) return;

    stopAnimationCore();

    int new_end = end_;
    int new_shot = shot_;
    bool const new_end_start = false;

    if (shot_ == static_cast<int>(dc::GameState::kShotPerEnd - 1)) {
        if (animInfo() == AnimInfo::kFinish) {
            // エンド最終ターンで最終フレームなので次のエンドに行く
            ++new_end;
        } else {
            // エンド終了ターンで最終フレームに行って無かったら最終フレームに飛ばすだけ
            setFrame(std::numeric_limits<int>::max());
            return;
        }
    } else {
        // エンド終了ターンでは無い場合は，このエンドの終了ターンに飛ぶ
        new_shot = dc::GameState::kShotPerEnd - 1;
    }

    // エンドの途中で終わっている可能性があるので制限をかける
    assert(game_log_->gameStates().size() >= 2);
    auto const clamped_turn_index = std::min(
                GetTurnIndex(new_end, new_shot),
                game_log_->gameStates().size() - 2);
    new_end = GetEndFromTurnIndex(clamped_turn_index);
    new_shot = GetShotFromTurnIndex(clamped_turn_index);

    setTurnCore(new_end, new_shot, new_end_start);
    setShotFrameCore(std::numeric_limits<int>::max());
    emitStateUpdated();
}

void Model::goPreviousEnd()
{
    if (!isLogOpened()) return;

    stopAnimationCore();

    int new_end = end_;
    int new_shot;
    bool new_end_start;

    if (end_ == 0) {
        new_shot = 0;
        new_end_start = true;
    } else {
        --new_end;
        new_shot = static_cast<int>(dc::GameState::kShotPerEnd - 1);
        new_end_start = false;
    }

    setTurnCore(new_end, new_shot, new_end_start);
    setShotFrameCore(std::numeric_limits<int>::max());
    emitStateUpdated();
}

void Model::reloadSettings()
{
    if (!isLogOpened()) return;

    stopAnimationCore();

    settings_.load();

    dirty_flag_.turn = true;

    emitStateUpdated();
}


void Model::emitStateUpdated()
{
    if (dirty_flag_.game_log || dirty_flag_.turn || dirty_flag_.animation) {
        emit stateUpdated(dirty_flag_);
        dirty_flag_.setAll(false);
    }
}

void Model::resetAll()
{
    stopAnimationCore();

    file_dir_.clear();
    game_log_.reset();
    shot_log_.reset();

    dirty_flag_.setAll(true);

    // overall_state_
    file_name_.clear();
    protocol_version_.clear();
    for (auto & team_name : team_name_) {
        team_name.clear();
    }
    game_id_.clear();
    date_time_.clear();

    // turn_state_
    end_ = 0;
    shot_ = 0;
    end_start_ = true;
    for (auto & team_stone_info : stones_info_) {
        team_stone_info.clear();
    }

    // anim_state_
    current_shot_frame_ = 0;
    current_shot_time_ = 0.f;
    sheet_drawer_.reset();
}

void Model::startAnimationCore()
{
    if (!isLogOpened()) return;
    if (animation_timer_.isActive()) return;

    // ショットログが読み込めていない場合，直接最終フレームに行く
    if (!shot_log_) {
        setFrame(std::numeric_limits<int>::max());
        return;
    }

    // アニメーション秒数が0の場合再生しない．
    if (shot_log_->isAnimationLengthZero()) return;

    animation_count_ = 0;
    animation_started_frame_ = current_shot_frame_;

    elapsed_timer_.start();
    animation_timer_.start();

    dirty_flag_.turn = true;
    emitStateUpdated();
}

void Model::stopAnimationCore()
{
    if (!animation_timer_.isActive()) return;

    animation_timer_.stop();
    elapsed_timer_.invalidate();

    dirty_flag_.turn = true;
}

void Model::setShotFrameCore(int frame)
{
    assert(isLogOpened());
    assert(frame >= 0);

    auto const pre_anim_info = animInfo();
    bool finish;

    if (frame < totalShotFrame()) {
        current_shot_frame_ = frame;
        current_shot_time_ = qBound(
                    0.f,
                    static_cast<float>(current_shot_frame_ * kAnimationSecondsPerFrame),
                    totalShotTime());
        finish = false;
    } else {
        current_shot_frame_ = totalShotFrame();
        current_shot_time_ = totalShotTime();
        finish = true;
    }

    // ストーンの位置を更新する
    sheet_drawer_.prepare(shot_log_.get(), current_shot_time_, game_state_stones_, finish);

    if (pre_anim_info != animInfo()) {
        // AnimInfo が変更された場合 (特にアニメーション終了状態に変化があった場合)，スコア表示等を更新する
        dirty_flag_.turn = true;
    }

    dirty_flag_.animation = true;
}

void Model::setTurnCore(int end, int shot, bool end_start, bool force_reload)
{
    assert(isLogOpened());

    size_t const turn_index = GetTurnIndex(end, shot);
    assert(turn_index < game_log_->gameStates().size() - 1);

    if (end_start) {
        assert(shot == 0);
    }

    // end, shot, end_startの変更チェック
    if (!force_reload
            && end == end_
            && shot == shot_
            && end_start == end_start_) {
        return;
    }

    shot_log_.reset();

    dirty_flag_.turn = true;
    dirty_flag_.animation = true;

    end_ = end;
    shot_ = shot;
    end_start_ = end_start;

    // TODO turn_state_ へロード


    // stone_info_ の更新
    if (end_start) {
        for (auto & team_si : stones_info_) {
            for (auto & si : team_si) {
                si = StoneInfo::kBeforeShot;
            }
        }
    } else {
        auto const& current_game_state = game_log_->gameStates().at(turn_index);
        dc::Team const hammer = current_game_state.hammer;
        for (int i = 0; i < turnsPerEnd(); ++i) {
            dc::Team i_team = static_cast<dc::Team>(i % 2);
            if (hammer == dc::Team::k0) {
                i_team = dc::GetOpponentTeam(i_team);
            }
            int const i_stone = i / 2;
            auto & si = stones_info_.at(static_cast<size_t>(i_team)).at(i_stone);

            if (i < shot_) {
                si = StoneInfo::kAfterShot;
            } else if (i == shot_) {
                si = StoneInfo::kInShot;
            } else {
                si = StoneInfo::kBeforeShot;
            }
        }
    }

    // shot_log_, total_time_, total_frame_ の読み込み
    // エンド開始時はショットログを読み込まない
    if (!end_start) {
        try {
            // ショットログファイル名を作成
            std::ostringstream file_name_buf;
            file_name_buf << "shot_e" << std::setw(3) << std::setfill('0') << end
                << "s" << std::setw(2) << std::setfill('0') << shot << ".json";

            // ロード
            shot_log_ = std::make_unique<ShotLog>(file_dir_ / file_name_buf.str());

        } catch (std::exception & e) {
            emit errorOccured("Error on opening shot log: " % QString::fromLatin1(e.what()));
        }

        game_state_stones_ = TransformStones(game_log_->gameStates().at(turn_index + 1).stones,
                                             dc::coordinate::GetShotSide(end_),
                                             dc::coordinate::Id::kSimulation);
    } else {
        game_state_stones_ = dc::GameState::Stones();
    }


    setShotFrameCore(0);
}

digitalcurling3::GameState const& Model::showingGameStateForScore() const
{
    if (game_log_) {
        if (settings_.show_game_result || isGameOver()) {
            return game_log_->gameStates().back();
        } else {
            auto turn_index = GetTurnIndex(end_, shot_);
            if (animInfo() == AnimInfo::kFinish) {
                ++turn_index;
            }
            return game_log_->gameStates().at(turn_index);
        }
    } else {
        return GetDefaultGameState();
    }
}

} // namespace digitalcurling3_gui
