
#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Map Map;
typedef ElvenUtils::Point Point;

const auto comparator = [](const auto &left, const auto &right) { return left < right; };
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

void tilt_west(Map &map, std::vector<Point> &rocks) {
    std::ranges::sort(rocks.begin(), rocks.end(), [](const auto &left, const auto &right) { return left.x < right.x; });
    for (auto &rock: rocks) {
        while (rock.x != 0 && map.at(rock.west(), rock.y) == EMPTY) {
            map.set(rock, EMPTY);
            rock.x = rock.west();
            map.set(rock, ROCK);
        }
    }
}

void tilt_south(Map &map, std::vector<Point> &rocks) {
    std::ranges::sort(rocks.begin(), rocks.end(), [](const auto &left, const auto &right) { return right.y < left.y; });
    for (auto &rock: rocks) {
        while (rock.y < map.y_size() - 1 && map.at(rock.x, rock.south()) == EMPTY) {
            map.set(rock, EMPTY);
            rock.y = rock.south();
            map.set(rock, ROCK);
        }
    }
}

void tilt_east(Map &map, std::vector<Point> &rocks) {
    std::ranges::sort(rocks.begin(), rocks.end(), [](const auto &left, const auto &right) { return right.x < left.x; });
    for (auto &rock: rocks) {
        while (rock.x < map.x_size() - 1 && map.at(rock.east(), rock.y) == EMPTY) {
            map.set(rock, EMPTY);
            rock.x = rock.east();
            map.set(rock, ROCK);
        }
    }
}

void round(Map &map, std::vector<Point> &rocks, std::vector<std::vector<Point>> &states) {
    std::vector<Point> prev_state;
    std::ranges::copy(rocks.begin(), rocks.end(), std::back_inserter(prev_state));
    std::ranges::sort(prev_state.begin(), prev_state.end(), comparator);
    states.emplace_back(prev_state);

    tilt_north(map, rocks);
    tilt_west(map, rocks);
    tilt_south(map, rocks);
    tilt_east(map, rocks);
}


std::size_t total_load(Map &map, const std::vector<Point> &rocks) {
    std::size_t load = 0;
    for (const auto & rock : rocks) {
        map.set(rock, ROCK);
        load += map.y_size() - rock.y;
    }
    return load;
}

// Using Floyd https://en.wikipedia.org/wiki/Cycle_detection#Floyd's_tortoise_and_hare
std::size_t solve(const ElvenIO::input_type &input) {
    auto map = Map(input);
    auto rocks = map.find_all(ROCK);
    std::vector<std::vector<Point>> states;
    auto tortoise = 1;
    auto hare = 2;
    round(map, rocks, states);
    round(map, rocks, states);
    round(map, rocks, states);
    for (;states[tortoise] != states[hare]; ++tortoise, hare += 2) {
        round(map, rocks, states);
        round(map, rocks, states);
    }
    auto mu = 0;
    tortoise = 0;
    for (;states[tortoise] != states[hare]; ++tortoise, ++hare, ++mu) {
        round(map, rocks, states);
    }
    auto lam = 1;
    hare = tortoise + 1;
    for (;states[tortoise] != states[hare]; ++hare, ++lam) { }
    return total_load(map, states[mu + (1000000000 - mu) % lam]);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 10);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}