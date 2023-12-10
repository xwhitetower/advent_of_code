#include <regex>
#include <set>

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

const std::set north_connections{VERTICAL_PIPE, NORTH_EAST_PIPE, NORTH_WEST_PIPE, START_MARKER};
const std::set south_connections{VERTICAL_PIPE, SOUTH_EAST_PIPE, SOUTH_WEST_PIPE, START_MARKER};
const std::set west_connections{HORIZONTAL_PIPE, NORTH_WEST_PIPE, SOUTH_WEST_PIPE, START_MARKER};
const std::set east_connections{HORIZONTAL_PIPE, NORTH_EAST_PIPE, SOUTH_EAST_PIPE, START_MARKER};

void explore(
    const Point &current_pipe, const Point &previous_pipe, const Distance &distance,
    const ElvenUtils::Map &map,  std::deque<PipePath> &pipes_to_explore) {

    if (
        map.in_y_boundary(current_pipe.north()) && previous_pipe.is_not(current_pipe.x, current_pipe.north())
        && south_connections.contains(map.at(current_pipe.x, current_pipe.north()))
        && north_connections.contains(map.at(current_pipe.x,current_pipe.y))
    ) {
        pipes_to_explore.emplace_back(Point(current_pipe.x, current_pipe.north()), current_pipe, distance + 1);
    }
    if (
        map.in_y_boundary(current_pipe.south()) && previous_pipe.is_not(current_pipe.x, current_pipe.south())
        && north_connections.contains(map.at(current_pipe.x, current_pipe.south()))
        && south_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        pipes_to_explore.emplace_back(Point(current_pipe.x, current_pipe.south()), current_pipe, distance + 1);
    }
    if (
        map.in_x_boundary(current_pipe.west()) && previous_pipe.is_not(current_pipe.west(), current_pipe.y)
        && east_connections.contains(map.at(current_pipe.west(), current_pipe.y))
        && west_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        pipes_to_explore.emplace_back(Point(current_pipe.west(), current_pipe.y), current_pipe, distance + 1);
    }
    if (
        map.in_x_boundary(current_pipe.east()) && previous_pipe.is_not(current_pipe.east(), current_pipe.y)
        && west_connections.contains(map.at(current_pipe.east(), current_pipe.y))
        && east_connections.contains(map.at(current_pipe.x, current_pipe.y))
    ) {
        pipes_to_explore.emplace_back(Point(current_pipe.east(), current_pipe.y), current_pipe, distance + 1);
    }
}

Distance farthest_loop_distance(const Point &start_position, const ElvenUtils::Map &map) {
    std::deque<PipePath> pipes_to_explore;
    explore(start_position, start_position, 0, map, pipes_to_explore);
    while (std::get<0>(pipes_to_explore.front()) != std::get<0>(pipes_to_explore.back())) {
        auto [current_pipe, previous_pipe, distance] = pipes_to_explore.front();
        pipes_to_explore.pop_front();
        explore(current_pipe, previous_pipe, distance, map, pipes_to_explore);
    }
    return std::min(std::get<2>(pipes_to_explore.front()), std::get<2>(pipes_to_explore.back()));
}

std::size_t solve(const ElvenIO::input_type &input) {
    ElvenUtils::Map map(input);
    return farthest_loop_distance(map.find(START_MARKER), map);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}