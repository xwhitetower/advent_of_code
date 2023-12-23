#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Point Point;
typedef ElvenUtils::Map ElfMap;
typedef std::vector<Point> Galaxies;
typedef std::size_t Distance;

constexpr char GALAXY_MARKER = '#';
constexpr std::size_t MILLION = 1000000;


void expand_x(Galaxies &galaxies, const std::size_t multiplier) {
    std::ranges::sort(
        galaxies.begin(), galaxies.end(),
        [](const auto &left, const auto &right) { return left.x < right.x; }
    );
    for (int i = 1; i < galaxies.size(); ++i) {
        if (const auto diff = galaxies[i].x - galaxies[i -1].x; diff > 1) {
            for (auto j = i; j < galaxies.size(); ++j) {
                galaxies[j].x += (diff - 1) * multiplier;
            }
        }
    }
}

void expand_y(Galaxies &galaxies, const std::size_t multiplier) {
    std::ranges::sort(
        galaxies.begin(), galaxies.end(),
        [](const auto &left, const auto &right) { return left.y < right.y; }
    );
    for (int i = 1; i < galaxies.size(); ++i) {
        if (const auto diff = galaxies[i].y - galaxies[i -1].y; diff > 1) {
            for (auto j = i; j < galaxies.size(); ++j) {
                galaxies[j].y += (diff - 1) * multiplier;
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

Distance part1(const ElvenIO::input_type &input) {
    const ElfMap map(input);
    auto galaxies = map.find_all(GALAXY_MARKER);
    expand_x(galaxies, 1);
    expand_y(galaxies, 1);
    return galactic_distance(galaxies);
}

Distance part2(const ElvenIO::input_type &input) {
    const ElfMap map(input);
    auto galaxies = map.find_all(GALAXY_MARKER);
    expand_x(galaxies, MILLION - 1);
    expand_y(galaxies, MILLION - 1);
    return galactic_distance(galaxies);
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 10);
    reporter.add_report(1, result1, solution1_time);
    auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 10);
    reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}