#include "game_log.hpp"

#include <stdexcept>
#include <QDebug>
#include <QFile>

namespace digitalcurling3_gui {

namespace dc = digitalcurling3;

GameLog::GameLog(QString const& file_name)
    : protocol_version_major_(0)
    , protocol_version_minor_(0)
    , game_id_()
    , date_time_()
    , game_setting_()
    , team_names_()
    , turns_per_end_(0)
    , game_states_()
    , move_results_()
    , error_message_()
{
    using namespace std::string_view_literals;

    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("file open failed");
    }

    dc::Move last_input_move;

    QTextStream file_stream(&file);

    while (!file_stream.atEnd()) {
        std::string line = file_stream.readLine().toStdString();
        nlohmann::json const j = nlohmann::json::parse(line);
        auto const tag = j.at("tag").get<std::string>();
        if (tag == "err") {
            error_message_ = QString::fromStdString(j.at("log").get<std::string>());
            break;

        } else if (tag == "gam") {
            auto const& j_log = j.at("log");

            auto const cmd = j_log.at("cmd").get<std::string>();
            if (cmd == "dc") {
                j_log.at("version").at("major").get_to(protocol_version_major_);
                j_log.at("version").at("minor").get_to(protocol_version_minor_);
                game_id_ = QString::fromStdString(j_log.at("game_id").get<std::string>());
                date_time_ = QString::fromStdString(j_log.at("date_time").get<std::string>());

            } else if (cmd == "is_ready") {
                if (j_log.at("game").at("rule").get<std::string>() == "normal") { // 通常ルール
                    turns_per_end_ = 16;
                } else {
                    throw std::runtime_error("unexpected rule");
                }

                j_log.at("game").at("setting").get_to(game_setting_);
                simulator_type_ = QString::fromStdString(j_log.at("game").at("simulator").at("type").get<std::string>());

            } else if (cmd == "new_game") {
                team_names_[0] = QString::fromStdString(j_log.at("name").at("team0").get<std::string>());
                team_names_[1] = QString::fromStdString(j_log.at("name").at("team1").get<std::string>());

            } else if (cmd == "update") {
                dc::GameState game_state;
                j_log.at("state").get_to(game_state);
                game_states_.emplace_back(std::move(game_state));
                auto const& j_log_last_move = j_log.at("last_move");
                if (!j_log_last_move.is_null()) {
                    MoveResult move_result;
                    j_log_last_move.at("actual_move").get_to(move_result.actual_move);
                    j_log_last_move.at("free_guard_zone_foul").get_to(move_result.free_guard_zone_foul);
                    move_result.input_move = last_input_move;
                    move_results_.emplace_back(std::move(move_result));
                }

            } else if (cmd == "move") {
                j_log.at("move").get_to(last_input_move);

            } else if (cmd == "game_over") {
                break;
            }
        }
    }


    /////////////////////////////////////////////////////////////

    // std::ifstream file(file_name);
    // if (!file) {
    //     throw std::runtime_error("file open failed");
    // }

    // dc::Move last_input_move;

    // std::string line;
    // while (std::getline(file, line)) {
    //     if (line.empty()) continue;

    //     nlohmann::json const j = nlohmann::json::parse(line);
    //     auto const tag = j.at("tag").get<std::string>();
    //     if (tag == "err") {
    //         error_message_ = j.at("log").get<std::string>();
    //         break;

    //     } else if (tag == "gam") {
    //         auto const& j_log = j.at("log");

    //         auto const cmd = j_log.at("cmd").get<std::string>();
    //         if (cmd == "dc") {
    //             j_log.at("version").at("major").get_to(protocol_version_major_);
    //             j_log.at("version").at("minor").get_to(protocol_version_minor_);
    //             j_log.at("game_id").get_to(game_id_);
    //             j_log.at("date_time").get_to(date_time_);

    //         } else if (cmd == "is_ready") {
    //             if (j_log.at("game").at("rule").get<std::string>() == "normal") { // 通常ルール
    //                 turns_per_end_ = 16;
    //             } else {
    //                 throw std::runtime_error("unexpected rule");
    //             }

    //             j_log.at("game").at("setting").get_to(game_setting_);
    //             j_log.at("game").at("simulator").at("type").get_to(simulator_type_);

    //         } else if (cmd == "new_game") {
    //             j_log.at("name").at("team0").get_to(team_names_[0]);
    //             j_log.at("name").at("team1").get_to(team_names_[1]);

    //         } else if (cmd == "update") {
    //             dc::GameState game_state;
    //             j_log.at("state").get_to(game_state);
    //             game_states_.emplace_back(std::move(game_state));
    //             auto const& j_log_last_move = j_log.at("last_move");
    //             if (!j_log_last_move.is_null()) {
    //                 MoveResult move_result;
    //                 j_log_last_move.at("actual_move").get_to(move_result.actual_move);
    //                 j_log_last_move.at("free_guard_zone_foul").get_to(move_result.free_guard_zone_foul);
    //                 move_result.input_move = last_input_move;
    //                 move_results_.emplace_back(std::move(move_result));
    //             }

    //         } else if (cmd == "move") {
    //             j_log.at("move").get_to(last_input_move);

    //         } else if (cmd == "game_over") {
    //             break;
    //         }
    //     }
    // }

    // if (game_states_.size() - 1 != move_results_.size()) {
    //     throw std::runtime_error("format error");
    // }
}

GameLog::~GameLog() = default;

} // namespace digitalcurling3_gui
