#include <deque>
#include <set>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Point Point;
typedef size_t Distance;
typedef std::tuple<Point, Point, Distance> PipePath;

constexpr char START_MARKER = 'S';
constexpr char VERTICAL_PIPE = '|';
constexpr char HORIZONTAL_PIPE = '-';
constexpr char NORTH_EAST_PIPE = 'L';
constexpr char NORTH_WEST_PIPE = 'J';
constexpr char SOUTH_WEST_PIPE = '7';
constexpr char SOUTH_EAST_PIPE = 'F';
constexpr char EMPTY_MARK = '.';
constexpr char VISITED_MARK = 'x';
constexpr char PAINTED_MARK = '#';

const std::set north_connections{VERTICAL_PIPE, NORTH_EAST_PIPE, NORTH_WEST_PIPE, START_MARKER};
const std::set south_connections{VERTICAL_PIPE, SOUTH_EAST_PIPE, SOUTH_WEST_PIPE, START_MARKER};
const std::set west_connections{HORIZONTAL_PIPE, NORTH_WEST_PIPE, SOUTH_WEST_PIPE, START_MARKER};
const std::set east_connections{HORIZONTAL_PIPE, NORTH_EAST_PIPE, SOUTH_EAST_PIPE, START_MARKER};

void explore(
    const Point &current_pipe, const Point &previous_pipe, const Distance &distance,
    const ElvenUtils::Map &map, std::deque<PipePath> &pipes_to_explore, ElvenUtils::Map &painted_map) {

    if (
        map.in_y_boundary(current_pipe.north()) && previous_pipe.is_not(current_pipe.x, current_pipe.north())
        && south_connections.contains(map.at(current_pipe.x, current_pipe.north()))
        && north_connections.contains(map.at(current_pipe.x,current_pipe.y))
    ) {
        const auto next_pipe = Point(current_pipe.x, current_pipe.north());
        painted_map.set(next_pipe, VISITED_MARK);
        pipes_to_explore.emplace_back(next_pipe, current_pipe, distance + 1);
    }
    if (
        map.in_y_boundary(current_pipe.south()) && previous_pipe.is_not(current_pipe.x, current_pipe.south())
        && north_connections.contains(map.at(current_pipe.x, current_pipe.south()))
        && south_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        const auto next_pipe = Point(current_pipe.x, current_pipe.south());
        painted_map.set(next_pipe, VISITED_MARK);
        pipes_to_explore.emplace_back(next_pipe, current_pipe, distance + 1);
    }
    if (
        map.in_x_boundary(current_pipe.west()) && previous_pipe.is_not(current_pipe.west(), current_pipe.y)
        && east_connections.contains(map.at(current_pipe.west(), current_pipe.y))
        && west_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        const auto next_pipe = Point(current_pipe.west(), current_pipe.y);
        painted_map.set(next_pipe, VISITED_MARK);
        pipes_to_explore.emplace_back(next_pipe, current_pipe, distance + 1);
    }
    if (
        map.in_x_boundary(current_pipe.east()) && previous_pipe.is_not(current_pipe.east(), current_pipe.y)
        && west_connections.contains(map.at(current_pipe.east(), current_pipe.y))
        && east_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        const auto next_pipe = Point(current_pipe.east(), current_pipe.y);
        painted_map.set(next_pipe, VISITED_MARK);
        pipes_to_explore.emplace_back(next_pipe, current_pipe, distance + 1);
    }
}

ElvenUtils::Map expand_map(const ElvenUtils::Map &map) {
    std::vector<std::vector<char>> expanded_map;
    expanded_map.reserve(map.y_size() * 2 + 2);
    expanded_map.emplace_back(map.x_size() * 2 + 2, EMPTY_MARK);
    // Ugly but works for now
    for (int y = 0; y < map.y_size(); ++y) {
        std::vector<char> new_row;
        new_row.reserve(map.x_size() * 2 + 2);
        new_row.push_back(EMPTY_MARK);
        for(int x = 0; x < map.x_size(); ++x) {
            const auto cell = map.at(x, y);
            new_row.push_back(cell);
            if (map.at(x, y) != EMPTY_MARK && map.in_x_boundary(x + 1)
                && west_connections.contains(map.at(x + 1, y))
                && east_connections.contains(map.at(x, y))
            ) {
                new_row.push_back(HORIZONTAL_PIPE);
            } else {
                new_row.push_back(EMPTY_MARK);
            }
        }
        new_row.push_back(EMPTY_MARK);
        expanded_map.emplace_back(new_row);
        std::vector<char> another_new_row;
        another_new_row.reserve(map.x_size() * 2 + 2);
        another_new_row.push_back(EMPTY_MARK);
        for(int x = 0; x < map.x_size(); ++x) {
            if (map.at(x, y) != EMPTY_MARK && map.in_y_boundary(y + 1)
                && north_connections.contains(map.at(x, y + 1))
                && south_connections.contains(map.at(x, y))
            ) {
                another_new_row.push_back(VERTICAL_PIPE);
            } else {
                another_new_row.push_back(EMPTY_MARK);
            }
            another_new_row.push_back(EMPTY_MARK);
        }
        another_new_row.push_back(EMPTY_MARK);
        expanded_map.emplace_back(another_new_row);
    }
    expanded_map.emplace_back(map.x_size() * 2 + 2, EMPTY_MARK);
    return ElvenUtils::Map(expanded_map);
}

void paint_outside_cells(ElvenUtils::Map &map) {
    std::deque<Point> explore_cells;
    explore_cells.emplace_back(0, 0);
    std::set<Point> visited_cells;
    map.set(0, 0, PAINTED_MARK);
    while (!explore_cells.empty()) {
        const auto point = explore_cells.front();
        explore_cells.pop_front();
        if (const auto north = Point(point.x, point.north());
            map.in_y_boundary(north.y) && map.at(north) != VISITED_MARK && !visited_cells.contains(north)
        ) {
            explore_cells.push_back(north);
            map.set(point.x, point.north(), PAINTED_MARK);
            visited_cells.insert(north);
        }
        if (const auto south = Point(point.x, point.south());
            map.in_y_boundary(south.y) && map.at(south) != VISITED_MARK && !visited_cells.contains(south)
        ) {
            explore_cells.push_back(south);
            map.set(point.x, point.south(), PAINTED_MARK);
            visited_cells.insert(south);
        }
        if (const auto east = Point(point.east(), point.y);
            map.in_x_boundary(east.x) && map.at(east) != VISITED_MARK && !visited_cells.contains(east)
        ) {
            explore_cells.push_back(east);
            map.set(point.east(), point.y, PAINTED_MARK);
            visited_cells.insert(east);
        }
        if (const auto west = Point(point.west(), point.y);
            map.in_x_boundary(west.x) && map.at(west) != VISITED_MARK && !visited_cells.contains(west)
        ) {
            explore_cells.push_back(west);
            map.set(point.west(), point.y, PAINTED_MARK);
            visited_cells.insert(west);
        }
    }
}


ElvenUtils::Map contract_map(const ElvenUtils::Map &map) {
    std::vector<std::vector<char>> contracted_map;
    contracted_map.reserve(map.y_size() / 2 - 1);
    for (int y = 1; y < map.y_size() - 1; y += 2) {
        std::vector<char> new_row;
        new_row.reserve(map.x_size() / 2 - 1);
        for (int x = 1; x < map.x_size() - 1; x += 2) {
            new_row.push_back(map.at(x, y));
        }
        contracted_map.emplace_back(new_row);
    }
    return ElvenUtils::Map(contracted_map);
}

size_t count_trapped_cells(const Point &start_position, const ElvenUtils::Map &map) {
    ElvenUtils::Map painted_map(map);
    std::deque<PipePath> pipes_to_explore;
    painted_map.set(start_position, VISITED_MARK);
    explore(start_position, start_position, 0, map, pipes_to_explore, painted_map);
    while (std::get<0>(pipes_to_explore.front()) != std::get<0>(pipes_to_explore.back())) {
        auto [current_pipe, previous_pipe, distance] = pipes_to_explore.front();
        pipes_to_explore.pop_front();
        explore(current_pipe, previous_pipe, distance, map, pipes_to_explore, painted_map);
    }
    paint_outside_cells(painted_map);
    const auto contracted_map = contract_map(painted_map);
    return std::transform_reduce(
        contracted_map.map.begin(), contracted_map.map.end(),
        0,
        std::plus(),
        [](const auto &row) {
            return std::count_if(row.begin(), row.end(),
                [](const auto cell) {
                    return cell != VISITED_MARK && cell != PAINTED_MARK;
            });
        }
    );
}

std::size_t solve(const ElvenIO::input_type &input) {
    const ElvenUtils::Map map(input);
    const auto expanded_map = expand_map(map);
    return count_trapped_cells(expanded_map.find(START_MARKER), expanded_map);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}