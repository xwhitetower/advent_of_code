#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

typedef std::string MapRow;
typedef std::vector<size_t> Clusters;
typedef std::pair<MapRow, Clusters> MapInfo;

constexpr char DAMAGED_SPRING = '#';
constexpr char UNKNOWN_SPRING = '?';
constexpr char EMPTY = ElvenUtils::Map::EMPTY;
const std::string POSSIBLE_DAMAGED = "#?";

MapInfo parse_line(const std::string &line) {
    std::stringstream stream;
    stream << line;
    std::string map_string, cluster_string_list;
    stream >> map_string >> cluster_string_list;
    std::stringstream cluster_stream;
    cluster_stream << cluster_string_list;
    std::string number_string;
    Clusters clusters;
    while(std::getline(cluster_stream, number_string, ',')) {
        clusters.emplace_back(std::stol(number_string));
    }
    return std::move(std::make_pair(map_string, clusters));
}

std::size_t brute_force_permutation_count(const std::string_view map, const Clusters &clusters, const size_t position) {
    if (position == clusters.size()) {
        if (map.find(DAMAGED_SPRING) == std::string::npos) {
            return 1;
        }
        return 0;
    }
    if (map.empty()) {
        return 0;
    }

    const auto next_slot = map.find_first_of(POSSIBLE_DAMAGED);
    if (next_slot == std::string::npos) {
        return 0;
    }
    const auto next_empty = next_slot + clusters[position];
    if (next_empty - 1 >= map.size()) {
        return 0;
    }
    bool has_damaged = false;
    for (int i = next_slot; i < next_slot + clusters[position]; ++i) {
        has_damaged = has_damaged || map[i] == DAMAGED_SPRING;
        if (map[i] == EMPTY) {
            if (i + 1 < map.size() && !has_damaged) {
                return brute_force_permutation_count(map.substr(i + 1), clusters, position);
            }
            return 0;
        }
    }
    if (next_empty < map.size() && map[next_empty] == DAMAGED_SPRING) {
        if (next_slot + 1 < map.size() && map[next_slot] != DAMAGED_SPRING) {
            return brute_force_permutation_count(map.substr(next_slot + 1), clusters, position);
        }
        return 0;
    }
    auto count = 0;
    if (next_empty + 1 < map.size()) {
        count += brute_force_permutation_count(map.substr(next_empty + 1), clusters, position + 1);
    } if (next_slot + 1 < map.size() && map[next_slot] != DAMAGED_SPRING) {
        count += brute_force_permutation_count(map.substr(next_slot + 1), clusters, position);
    } if (position + 1 == clusters.size() && (next_empty == map.size() || next_empty + 1 == map.size())) {
        count += brute_force_permutation_count("", clusters, position + 1);
    }

    return count;
}

std::size_t arrangament_count(const std::string &line) {
    auto [map, clusters] = parse_line(line);
    Clusters known_clusters;
    return brute_force_permutation_count(std::string_view(map), clusters, 0);
}

std::size_t solve(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(), input.end(),
        0,
        std::plus(),
        [](const auto &line) { return arrangament_count(line); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 1);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}