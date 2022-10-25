#ifndef DIGITALCURLING3_GUI_COORD_HPP
#define DIGITALCURLING3_GUI_COORD_HPP

#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_gui {

inline digitalcurling3::GameState::Stones TransformStones(digitalcurling3::GameState::Stones const& stones, digitalcurling3::coordinate::Id from, digitalcurling3::coordinate::Id to)
{
    digitalcurling3::GameState::Stones out;
    for (size_t i_team = 0; i_team < stones.size(); ++i_team) {
        for (size_t i_stone = 0; i_stone < stones[i_team].size(); ++i_stone) {
            if (stones[i_team][i_stone].has_value()) {
                out[i_team][i_stone].emplace();
                out[i_team][i_stone]->position = digitalcurling3::coordinate::TransformPosition(stones[i_team][i_stone]->position, from, to);
                out[i_team][i_stone]->angle = digitalcurling3::coordinate::TransformAngle(stones[i_team][i_stone]->angle, from, to);
            } else {
                out[i_team][i_stone] = std::nullopt;
            }
        }
    }
    return out;
}

} // namespace digitalcurling3_gui

#endif // DIGITALCURLING3_GUI_COORD_HPP
