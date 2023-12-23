#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"
#include "elven_utils.h"

typedef ElvenUtils::Map Map;

auto parse_input(const ElvenIO::input_type &input) {
    std::vector<Map> maps;
    std::vector<std::string> map;
    for (const auto& line : input) {
        if (line.empty()) {
            maps.emplace_back(map);
            map.clear();
        } else {
            map.emplace_back(line);
        }
    }
    maps.emplace_back(map);
    return maps;
}

std::optional<std::size_t> vertical_reflection_with_smudge(const Map &map, const int smudges) {
    for (std::size_t x = 0; x < map.x_size() - 1; ++x) {
        auto found_smudges = 0;
        for (size_t y = 0; found_smudges < smudges + 1 && y < map.y_size(); ++y) {
            for (int i = x, j = x + 1; found_smudges < smudges + 1 && i >= 0 && j <map.x_size(); --i, ++j) {
                if (map.at(i, y) != map.at(j, y)) {
                    found_smudges += 1;
                }
            }
        }
        if (found_smudges == smudges) { return {x}; }
    }
    return std::nullopt;
}

std::optional<std::size_t> horizontal_reflection_with_smudge(const Map &map, const int smudges) {
    for (int y = 0; y < map.y_size() - 1; ++y) {
        auto found_smudges = 0;
        for (int x = 0; found_smudges < smudges + 1 && x < map.x_size(); ++x) {
            for (int i = y, j = y + 1; found_smudges < smudges + 1 && i >= 0 && j < map.y_size(); --i, ++j) {
                if (map.at(x, i) != map.at(x, j)) { found_smudges += 1; }
            }
        }
        if (found_smudges == smudges) { return {y}; }
    }
    return std::nullopt;
}

std::size_t mirror_score(const Map &map, const int smudges) {
    if (const auto vertical_reflection_pos = vertical_reflection_with_smudge(map, smudges); vertical_reflection_pos.has_value()) {
        return vertical_reflection_pos.value() + 1;
    }
    if (const auto horizontal_reflection_pos = horizontal_reflection_with_smudge(map, smudges); horizontal_reflection_pos.has_value()) {
        return (horizontal_reflection_pos.value() + 1) * 100;
    }
    throw std::runtime_error("no reflection found");
}

std::size_t part1(const ElvenIO::input_type &input) {
    const auto maps = parse_input(input);
    return std::transform_reduce(
        maps.begin(), maps.end(),
        static_cast<size_t>(0),
        std::plus(),
        [](const auto &map) { return mirror_score(map, 0); }
    );
}

std::size_t part2(const ElvenIO::input_type &input) {
    const auto maps = parse_input(input);
    return std::transform_reduce(
        maps.begin(), maps.end(),
        static_cast<size_t>(0),
        std::plus(),
        [](const auto &map) { return mirror_score(map, 1); }
    );
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