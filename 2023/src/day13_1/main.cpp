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

bool is_vertical_reflection(const std::vector<char> &row, const int x) {
    for (int i = x, j = x + 1; i >= 0 && j < row.size(); --i, ++j) {
        if (row[i] != row[j]) { return false; }
    }
    return true;
}

bool is_horizontal_reflection(const Map &map, const int x, const int y) {
    for (int i = y, j = y + 1; i >= 0 && j < map.y_size(); --i, ++j) {
        if (map.at(x, i) != map.at(x, j)) { return false; }
    }
    return true;
}

std::optional<std::size_t> vertical_reflection(const Map &map) {
    for (int x = 0; x < map.x_size() - 1; ++x) {
        if (std::ranges::all_of(map.map.begin(), map.map.end(), [&x](const auto &row){ return is_vertical_reflection(row, x); })) {
            return std::optional(x);
        }
    }
    return std::nullopt;
}

std::optional<std::size_t> horizontal_reflection(const Map &map) {
    for (int y = 0; y < map.y_size() - 1; ++y) {
        bool is_mirrored = true;
        for (int x = 0; x < map.x_size() && is_mirrored; ++x) {
            is_mirrored = is_horizontal_reflection(map, x, y);
        }
        if (is_mirrored) { return std::optional(y); }
    }
    return std::nullopt;
}

std::size_t mirror_score(const Map &map) {
    if (const auto vertical_reflection_pos = vertical_reflection(map); vertical_reflection_pos.has_value()) {
        return vertical_reflection_pos.value() + 1;
    }
    if (const auto horizontal_reflection_pos = horizontal_reflection(map); horizontal_reflection_pos.has_value()) {
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