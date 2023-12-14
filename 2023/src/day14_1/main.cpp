
#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Map Map;
typedef ElvenUtils::Point Point;

constexpr char ROCK = 'O';
constexpr char EMPTY = ElvenUtils::Map::EMPTY;

void tilt_north(Map &map, std::vector<Point> &rocks) {
    std::ranges::sort(rocks.begin(), rocks.end(), [](const auto &left, const auto &right) { return left.y < right.y; });
    for (auto &rock: rocks) {
        while (rock.y != 0 && map.at(rock.x, rock.north()) == EMPTY) {
            map.set(rock, EMPTY);
            rock.y = rock.north();
            map.set(rock, ROCK);
        }
    }
}


std::size_t total_load(Map &map, const std::vector<Point> &rocks) {
    std::size_t load = 0;
    for (const auto & rock : rocks) {
        map.set(rock, ROCK);
        load += map.y_size() - rock.y;
    }
    return load;
}

std::size_t solve(const ElvenIO::input_type &input) {
    auto map = Map(input);
    auto rocks = map.find_all(ROCK);
    tilt_north(map, rocks);
    return total_load(map, rocks);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}