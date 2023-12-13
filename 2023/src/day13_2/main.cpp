#include <__numeric/transform_reduce.h>

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

std::optional<std::size_t> vertical_reflection_with_smudge(const Map &map) {
    for (std::size_t x = 0; x < map.x_size() - 1; ++x) {
        auto smudges = 0;
        for (size_t y = 0; smudges < 2 && y < map.y_size(); ++y) {
            for (int i = x, j = x + 1; smudges < 2 && i >= 0 && j <map.x_size(); --i, ++j) {
                if (map.at(i, y) != map.at(j, y)) {
                    smudges += 1;
                }
            }
        }
        if (smudges == 1) { return {x}; }
    }
    return std::nullopt;
}

std::optional<std::size_t> horizontal_reflection_with_smudge(const Map &map) {
    for (int y = 0; y < map.y_size() - 1; ++y) {
        auto smudges = 0;
        for (int x = 0; smudges < 2 && x < map.x_size(); ++x) {
            for (int i = y, j = y + 1; smudges < 2 && i >= 0 && j < map.y_size(); --i, ++j) {
                if (map.at(x, i) != map.at(x, j)) { smudges += 1; }
            }
        }
        if (smudges == 1) { return {y}; }
    }
    return std::nullopt;
}

std::size_t mirror_score(const Map &map) {
    if (const auto vertical_reflection_pos = vertical_reflection_with_smudge(map); vertical_reflection_pos.has_value()) {
        return vertical_reflection_pos.value() + 1;
    }
    if (const auto horizontal_reflection_pos = horizontal_reflection_with_smudge(map); horizontal_reflection_pos.has_value()) {
        return (horizontal_reflection_pos.value() + 1) * 100;
    }
    throw std::runtime_error("no reflection found");
}

std::size_t solve(const ElvenIO::input_type &input) {
    const auto maps = parse_input(input);
    return std::transform_reduce(
        maps.begin(), maps.end(),
        static_cast<size_t>(0),
        std::plus(),
        [](const auto &map) { return mirror_score(map); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}