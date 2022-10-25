#ifndef DIGITALCURLING3_GUI_GAME_LOG_HPP
#define DIGITALCURLING3_GUI_GAME_LOG_HPP

#include <cstdint>
#include <string_view>
#include <array>
#include <vector>
#include <chrono>
#include <optional>
#include <filesystem>
#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_gui {

class GameLog
{
public:

    struct MoveResult {
        digitalcurling3::Move input_move; // ユーザーが送信したmove
        digitalcurling3::Move actual_move; // 実際に行われたmove
        bool free_guard_zone_foul;
    };

    GameLog(std::filesystem::path const& file_name);
    GameLog(GameLog const&) = delete;
    GameLog & operator = (GameLog const&) = delete;

    ~GameLog();

    std::uint32_t protocolVersionMajor() const { return protocol_version_major_; }
    std::uint32_t protocolVersionMinor() const { return protocol_version_minor_; }
    std::string const& gameId() const { return game_id_; }
    std::string const& dateTime() const { return date_time_; }
    digitalcurling3::GameSetting const& gameSetting() const { return game_setting_; }
    std::array<std::string, 2> const& teamNames() const { return team_names_; }

    // エンド毎のショット数
    std::uint8_t turnsPerEnd() const { return turns_per_end_; }

    //
    std::vector<digitalcurling3::GameState> const& gameStates() const { return game_states_; }

    //
    std::vector<MoveResult> const& moveResults() const { return move_results_; }

    //
    std::string const& simulatorType() const { return simulator_type_; }

    // エラーが発生した場合は std::nullopt でない．
    std::optional<std::string> errorMessage() const { return error_message_; }

private:
    std::uint32_t protocol_version_major_;
    std::uint32_t protocol_version_minor_;
    std::string game_id_;
    std::string date_time_;
    digitalcurling3::GameSetting game_setting_;
    std::array<std::string, 2> team_names_;
    std::uint8_t turns_per_end_;
    std::vector<digitalcurling3::GameState> game_states_;
    std::vector<MoveResult> move_results_;
    std::optional<std::string> error_message_;
    std::string simulator_type_;
};

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_GAME_LOG_HPP
