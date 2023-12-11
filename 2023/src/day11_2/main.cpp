#include <regex>
#include <set>

#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Point Point;
typedef ElvenUtils::Map ElfMap;
typedef std::vector<Point> Galaxies;
typedef std::size_t Distance;

constexpr char GALAXY_MARKER = '#';
constexpr std::size_t MILLION = 1000000;


void expand_x(Galaxies &galaxies) {
    std::ranges::sort(
        galaxies.begin(), galaxies.end(),
        [](const auto &left, const auto &right) { return left.x < right.x; }
    );
    for (int i = 1; i < galaxies.size(); ++i) {
        if (const auto diff = galaxies[i].x - galaxies[i -1].x; diff > 1) {
            for (auto j = i; j < galaxies.size(); ++j) {
                galaxies[j].x += (diff - 1) * (MILLION - 1);
            }
        }
    }
}

void expand_y(Galaxies &galaxies) {
    std::ranges::sort(
        galaxies.begin(), galaxies.end(),
        [](const auto &left, const auto &right) { return left.y < right.y; }
    );
    for (int i = 1; i < galaxies.size(); ++i) {
        if (const auto diff = galaxies[i].y - galaxies[i -1].y; diff > 1) {
            for (auto j = i; j < galaxies.size(); ++j) {
                galaxies[j].y += (diff - 1) * (MILLION - 1);
            }
        }
    }
}

Distance galactic_distance(const Galaxies &galaxies) {
    Distance distance = 0;
    for (int i = 0; i < galaxies.size(); ++ i) {
        for (int j = i + 1; j < galaxies.size(); ++j) {
            distance += galaxies[i].manhattan_distance(galaxies[j]);
        }
    }
    return distance;
}

Distance solve(const ElvenIO::input_type &input) {
    const ElfMap map(input);
    auto galaxies = map.find_all(GALAXY_MARKER);
    expand_x(galaxies);
    expand_y(galaxies);
    return galactic_distance(galaxies);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}