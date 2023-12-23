#include <map>
#include <queue>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

typedef ElvenUtils::Map Map;
typedef ElvenUtils::Point Point;

const Point RIGHT_DIRECTION = Point(1, 0);
const Point LEFT_DIRECTION = Point(-1, 0);
const Point UP_DIRECTION = Point(0, -1);
const Point DOWN_DIRECTION = Point(0, 1);

constexpr uint8_t NUM_MOVES = 3;
constexpr uint8_t CAN_TURN_AT_REMAINING_MOVES = NUM_MOVES;
constexpr uint8_t ULTRA_NUM_MOVES = 10;
constexpr uint8_t ULTRA_CAN_TURN_AT_REMAINING_MOVES = ULTRA_NUM_MOVES - 4;

struct CrucibleState {
    size_t heat_loss;
    Point position;
    size_t distance_to_goal;
    Point direction;
    uint8_t remaining_moves;

    CrucibleState(const size_t heat_loss, const Point& position, const size_t distance_to_goal, const Point& direction, const uint8_t remaining_moves)
        : heat_loss(heat_loss),
          position(position),
          distance_to_goal(distance_to_goal),
          direction(direction),
    remaining_moves(remaining_moves) {}


    bool operator<(const CrucibleState& other) const {
        return std::tie(distance_to_goal, heat_loss, position, direction, remaining_moves) <
               std::tie(other.distance_to_goal, other.heat_loss, other.position, other.direction, other.remaining_moves);
    }
    bool operator>(const CrucibleState& other) const {
        return std::tie(distance_to_goal, heat_loss, position, direction, remaining_moves) >
               std::tie(other.distance_to_goal, other.heat_loss, other.position, other.direction, other.remaining_moves);
    }
};

struct VisistedStates {
    std::map<Point, std::vector<std::vector<std::size_t>>> directional_state;

    explicit VisistedStates(const Map& map) {
        std::vector<std::vector<std::size_t>> down_locked;
        std::vector<std::vector<std::size_t>> up_locked;
        std::vector<std::vector<std::size_t>> left_locked;
        std::vector<std::vector<std::size_t>> right_locked;
        for (const auto & row : map.map) {
            std::vector heat_row(map.x_size(), std::numeric_limits<size_t>::max());
            down_locked.emplace_back(heat_row);
            up_locked.emplace_back(heat_row);
            left_locked.emplace_back(heat_row);
            right_locked.emplace_back(heat_row);
        }
        directional_state[UP_DIRECTION] = up_locked;
        directional_state[DOWN_DIRECTION] = down_locked;
        directional_state[LEFT_DIRECTION] = left_locked;
        directional_state[RIGHT_DIRECTION] = right_locked;

    }

    bool is_improvement(const Point& position, const Point& direction, const std::size_t heat_loss) {
        const auto& state = directional_state[direction];
        return heat_loss < state[position.y][position.x];
    }

    bool update(const Point& position, const Point& direction, const std::size_t heat_loss) {
        auto& state = directional_state[direction];
        return state[position.y][position.x] = heat_loss;
    }
};

typedef std::priority_queue<CrucibleState, std::vector<CrucibleState>, std::greater<>> CruciblePriorityQueue;


void explore(
    const CrucibleState& state, const Map& map, const Point& goal, const size_t min_heat,
    CruciblePriorityQueue &search_queue, VisistedStates &visisted_states,
    const int num_moves, const int can_turn_at_remaining_moves
    ) {
    if (const auto next_position = state.position + state.direction;
        state.remaining_moves > 0 && map.in_boundary(next_position) && state.heat_loss + map.at(next_position) < min_heat
    ) {
        search_queue.emplace(state.heat_loss + map.at(next_position), next_position, goal.manhattan_distance(next_position), state.direction, state.remaining_moves - 1);
    }
    if (state.remaining_moves <= can_turn_at_remaining_moves) {
        if (state.direction == UP_DIRECTION || state.direction == DOWN_DIRECTION) {
            if (
                const auto next_right_position = state.position + RIGHT_DIRECTION;
                map.in_boundary(next_right_position) && state.heat_loss + map.at(next_right_position) < min_heat && visisted_states.is_improvement(next_right_position, RIGHT_DIRECTION, state.heat_loss)
                ) {
                visisted_states.update(next_right_position, RIGHT_DIRECTION, state.heat_loss);
                search_queue.emplace(state.heat_loss + map.at(next_right_position), next_right_position, goal.manhattan_distance(next_right_position), RIGHT_DIRECTION, num_moves -1);
                }
            if (
                const auto next_left_position = state.position + LEFT_DIRECTION;
                map.in_boundary(next_left_position) && state.heat_loss + map.at(next_left_position) < min_heat && visisted_states.is_improvement(next_left_position, LEFT_DIRECTION, state.heat_loss)
            ) {
                visisted_states.update(next_left_position, LEFT_DIRECTION, state.heat_loss);
                search_queue.emplace(state.heat_loss + map.at(next_left_position), next_left_position, goal.manhattan_distance(next_left_position), LEFT_DIRECTION, num_moves -1);
            }
        } else if (state.direction == RIGHT_DIRECTION || state.direction == LEFT_DIRECTION) {
            if (
                const auto next_up_position = state.position + UP_DIRECTION;
                map.in_boundary(next_up_position) && state.heat_loss + map.at(next_up_position) < min_heat && visisted_states.is_improvement(next_up_position, UP_DIRECTION, state.heat_loss)
            ) {
                visisted_states.update(next_up_position, UP_DIRECTION, state.heat_loss);
                search_queue.emplace(state.heat_loss + map.at(next_up_position), next_up_position, goal.manhattan_distance(next_up_position), UP_DIRECTION, num_moves -1);
            }
            if (
                const auto next_down_position = state.position + DOWN_DIRECTION;
                map.in_boundary(next_down_position) && state.heat_loss + map.at(next_down_position) < min_heat && visisted_states.is_improvement(next_down_position, DOWN_DIRECTION, state.heat_loss)
            ) {
                visisted_states.update(next_down_position, DOWN_DIRECTION, state.heat_loss);
                search_queue.emplace(state.heat_loss + map.at(next_down_position), next_down_position, goal.manhattan_distance(next_down_position), DOWN_DIRECTION, num_moves -1);
            }
        }
    }
}

std::size_t min_heat_loss(const Map& map, const int num_moves, const int can_turn_at_remaining_moves) {
    VisistedStates visisted_states(map);
    CruciblePriorityQueue search_queue;
    const auto start = Point(0, 0);
    const auto goal = Point(map.x_size() - 1, map.y_size() - 1);
    search_queue.emplace(0, start, goal.manhattan_distance(start), RIGHT_DIRECTION, NUM_MOVES);
    search_queue.emplace(0, start, goal.manhattan_distance(start), DOWN_DIRECTION, NUM_MOVES);
    std::size_t min_heat = std::numeric_limits<size_t>::max();
    while (!search_queue.empty()) {
        const auto state = search_queue.top();
        search_queue.pop();
        if (state.position == goal) {
            if (state.remaining_moves <= CAN_TURN_AT_REMAINING_MOVES) {
                min_heat = std::min(min_heat, state.heat_loss);
            }
        } else if (state.heat_loss < min_heat) {
            explore(state, map, goal, min_heat, search_queue, visisted_states, num_moves, can_turn_at_remaining_moves);
        }
    }
    return min_heat;
}

std::size_t part1(const ElvenIO::input_type& input) {
    Map map(input);
    for (auto &row : map.map) {
        for (auto &cell : row) {
            cell -= '0';
        }
    }
    return min_heat_loss(map, NUM_MOVES, CAN_TURN_AT_REMAINING_MOVES);
}

std::size_t part2(const ElvenIO::input_type& input) {
    Map map(input);
    for (auto &row : map.map) {
        for (auto &cell : row) {
            cell -= '0';
        }
    }
    return min_heat_loss(map, ULTRA_NUM_MOVES, ULTRA_CAN_TURN_AT_REMAINING_MOVES);
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 1);
    reporter.add_report(1, result1, solution1_time);
    auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 1);
    reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}