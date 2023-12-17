
#include <deque>
#include <set>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

typedef ElvenUtils::Map Map;
typedef ElvenUtils::Point Point;
typedef std::pair<Point, Point> Beam;

constexpr char EMPTY = ElvenUtils::Map::EMPTY;
constexpr char ENERGIZED = ElvenUtils::Map::BLOCKED;
constexpr char RIGHT_UP_MIRROR = '/';
constexpr char RIGHT_DOWN_MIRROR = '\\';
constexpr char VERTICAL_SPLITTER = '|';
constexpr char HORIZONTAL_SPLITTER = '-';

const Point RIGHT_DIRECTION = Point(1, 0);
const Point LEFT_DIRECTION = Point(-1, 0);
const Point UP_DIRECTION = Point(0, -1);
const Point DOWN_DIRECTION = Point(0, 1);

void queue_new_position(const Map &map, const Point &beam_position, const Point &beam_direction, std::deque<Beam> &beams) {
    // This is is ugly
    if (const auto cell = map.at(beam_position); cell == RIGHT_UP_MIRROR) {
        if (beam_direction == LEFT_DIRECTION) {
            beams.emplace_back(beam_position, DOWN_DIRECTION);
        } else if (beam_direction == RIGHT_DIRECTION) {
            beams.emplace_back(beam_position, UP_DIRECTION);
        } else if (beam_direction == UP_DIRECTION) {
            beams.emplace_back(beam_position, RIGHT_DIRECTION);
        } else if (beam_direction == DOWN_DIRECTION) {
            beams.emplace_back(beam_position, LEFT_DIRECTION);
        }
    } else if (cell == RIGHT_DOWN_MIRROR) {
        if (beam_direction == LEFT_DIRECTION) {
            beams.emplace_back(beam_position, UP_DIRECTION);
        } else if (beam_direction == RIGHT_DIRECTION) {
            beams.emplace_back(beam_position, DOWN_DIRECTION);
        } else if (beam_direction == UP_DIRECTION) {
            beams.emplace_back(beam_position, LEFT_DIRECTION);
        } else if (beam_direction == DOWN_DIRECTION) {
            beams.emplace_back(beam_position, RIGHT_DIRECTION);
        }
    } else if (cell == VERTICAL_SPLITTER) {
        if (beam_direction == UP_DIRECTION || beam_direction == DOWN_DIRECTION) {
            beams.emplace_back(beam_position, beam_direction);
        } else {
            beams.emplace_back(beam_position, UP_DIRECTION);
            beams.emplace_back(beam_position, DOWN_DIRECTION);
        }
    } else if (cell == HORIZONTAL_SPLITTER) {
        if (beam_direction == LEFT_DIRECTION || beam_direction == RIGHT_DIRECTION) {
            beams.emplace_back(beam_position, beam_direction);
        } else {
            beams.emplace_back(beam_position, LEFT_DIRECTION);
            beams.emplace_back(beam_position, RIGHT_DIRECTION);
        }
    } else if (cell == EMPTY) {
        beams.emplace_back(beam_position, beam_direction);
    }
}

Map energize(const ElvenIO::input_type &input) {
    const Map map(input);
    Map energized(input);
    std::deque<Beam> beams;
    std::set<Beam> visited_states;
    beams.emplace_back(Point(-1, 0), RIGHT_DIRECTION);
    while (!beams.empty()) {
        auto [beam_position, beam_direction] = beams.front();
        beams.pop_front();
        auto [_, has_been_inserted] = visited_states.insert(Beam(beam_position, beam_direction));
        beam_position += beam_direction;
        if (has_been_inserted && map.in_boundary(beam_position)) {
            energized.set(beam_position, ENERGIZED);
            queue_new_position(map, beam_position, beam_direction, beams);
        }
    }

    return energized;
}

std::size_t solve(const ElvenIO::input_type &input) {
    const Map energized = energize(input);
    energized.print();
    return energized.find_all(ENERGIZED).size();
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}