#include <map>
#include <set>
#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_utils.h"
#include "elven_measure.h"

typedef std::string MapRow;

constexpr char DAMAGED_SPRING = '#';
constexpr char UNKNOWN_SPRING = '?';
constexpr char EMPTY = ElvenUtils::Map::EMPTY;
const std::string POSSIBLE_DAMAGED = "#?";

struct ClusterData {
    std::size_t start_pos;
    std::size_t end_pos;
    std::size_t size;
    std::size_t unknwon_count;

    ClusterData(const std::size_t start_pos, const std::size_t end_pos, const std::size_t size) : start_pos(start_pos), end_pos(end_pos), size(size), unknwon_count(0) {}
    ClusterData(const std::size_t start_pos, const std::size_t end_pos, const std::size_t size, const std::size_t unknwon_count) : start_pos(start_pos), end_pos(end_pos), size(size), unknwon_count(unknwon_count) {}
    bool operator<(const ClusterData& rhs) const {
        return std::tie(start_pos, end_pos, size) < std::tie(rhs.start_pos, rhs.end_pos, rhs.size);
    }
    bool operator==(const ClusterData& rhs) const {
        return std::tie(start_pos, end_pos, size) == std::tie(rhs.start_pos, rhs.end_pos, rhs.size);
    }
};

typedef std::vector<ClusterData> Clusters;
typedef std::vector<ClusterData> DamagedClusters;
typedef std::vector<std::size_t> CollisionMap;
typedef std::pair<MapRow, Clusters> MapInfo;

struct PermutationState {

    Clusters clusters;
    explicit PermutationState(const Clusters &clusters) : clusters(clusters) {}

    bool operator<(const PermutationState& other) const {
        for (int i = 0; i < clusters.size(); ++i) {
            if (clusters[i] < other.clusters[i]) {
                return true;
            }
        }
        return false;
    }
    bool operator==(const PermutationState& rhs) const {
        return clusters == rhs.clusters;
    }
};

struct Fragment {

    int map_start_pos;
    int map_end_pos;
    int clusters_start_pos;
    int clusters_end_pos;
    Fragment(const int map_start_pos, const int map_end_pos, const int clusters_start_pos, const int clusters_end_pos)
        : map_start_pos(map_start_pos), map_end_pos(map_end_pos), clusters_start_pos(clusters_start_pos), clusters_end_pos(clusters_end_pos) {}

    friend bool operator<(const Fragment& lhs, const Fragment& rhs) {
        return std::tie(lhs.map_start_pos, lhs.map_end_pos, lhs.clusters_start_pos, lhs.clusters_end_pos) < std::tie(rhs.map_start_pos, rhs.map_end_pos, rhs.clusters_start_pos, rhs.clusters_end_pos);
    }
};
typedef std::map<Fragment, std::size_t> FragmentCache;


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
        clusters.emplace_back(std::string::npos, std::string::npos, std::stol(number_string));
    }
    return std::move(std::make_pair(map_string, clusters));
}

std::vector<ClusterData> find_damaged(const std::string_view map) {
    std::vector<ClusterData> damaged_clusters;
    char prev = EMPTY;
    std::size_t unknown = 0;
    for (int i = 0; i < map.size(); ++i) {
        if (map[i] == DAMAGED_SPRING) {
            if (damaged_clusters.empty() || prev != DAMAGED_SPRING) {
                damaged_clusters.emplace_back(i, i, 1, unknown);
            } else {
                damaged_clusters.back().end_pos = i;
                damaged_clusters.back().size += 1;
            }
            unknown = 0;
        } else if (map[i] == UNKNOWN_SPRING) {
            ++unknown;
        }
        prev = map[i];
    }
    return damaged_clusters;
}

bool is_valid_assignment(
    const std::string_view map,
    const DamagedClusters &damaged_clusters,
    const CollisionMap &collisions
    ) {
    for (const auto &damaged_cluster: damaged_clusters) {
        for (auto i = damaged_cluster.start_pos; i <= damaged_cluster.end_pos; ++i) {
            if (collisions[i] != 1) { return false; }
        }
    }
    return true;
}

bool has_no_collisions(const std::size_t start_pos, const std::size_t end_pos, const CollisionMap &collisions) {
    for (std::size_t i = start_pos; i <= end_pos; ++i) {
        if (collisions[i] != 0) { return false; }
    }
    return true;
}

std::size_t brute_force_fragment_permutation_count(
    const std::string_view map, const Clusters &clusters, const Fragment &fragment, const int map_position, const int cluster_position
    ) {
    if (cluster_position > fragment.clusters_end_pos) {
        return 1;
    }
    if (map_position > fragment.map_end_pos) {
        return 0;
    }
    const auto next_slot = map.find_first_of(UNKNOWN_SPRING, map_position);
    if (next_slot == std::string::npos || next_slot > fragment.map_end_pos) {
        return 0;
    }
    const auto next_empty = next_slot + clusters[cluster_position].size;
    if (next_empty - 1 > fragment.map_end_pos) {
        return 0;
    }
    for (int i = next_slot; i < next_empty; ++i) {
        if (map[i] == EMPTY) {
            if (i + 1 <= fragment.map_end_pos) {
                return brute_force_fragment_permutation_count(map, clusters, fragment, i + 1, cluster_position);
            }
            return 0;
        }
    }
    std::size_t count = 0;
    if (next_empty + 1 <= fragment.map_end_pos) {
        count += brute_force_fragment_permutation_count(map, clusters, fragment, next_empty + 1, cluster_position + 1);
    } if (next_slot + 1 <= fragment.map_end_pos) {
        count += brute_force_fragment_permutation_count(map, clusters, fragment, next_slot + 1, cluster_position);
    } if (cluster_position == fragment.clusters_end_pos && (next_empty == fragment.map_end_pos || next_empty == fragment.map_end_pos + 1)) {
        count += brute_force_fragment_permutation_count(map, clusters, fragment, fragment.map_end_pos + 1, cluster_position + 1);
    }
    return count;
}

std::size_t permutation_count(const std::string_view map, const Clusters &clusters, FragmentCache &fragment_cache) {
    if (std::ranges::all_of(
        clusters.begin(), clusters.end(),
        [](const auto &cluster) { return cluster.start_pos != std::string::npos; })
    ) {
        return 1;
    }

    std::vector<Fragment> fragments;
    int next_end_pos = 0;
    int next_cluster_end_pos = 0;
    int assigned_clusters = 0;
    for (int i = 0; i < clusters.size(); ++i) {
        if (const auto cluster = clusters[i];  cluster.start_pos != std::string::npos) {
            if (i != 0 && cluster.start_pos > 1 && next_end_pos < map.size() && next_cluster_end_pos <= i -1) {
                fragments.emplace_back(next_end_pos, cluster.start_pos - 2, next_cluster_end_pos, i - 1);
            }
            next_end_pos = cluster.end_pos + 2;
            next_cluster_end_pos = i + 1;
            assigned_clusters++;
        }
    }
    if (fragments.empty()) {
        fragments.emplace_back(next_end_pos, map.size() - 1, next_cluster_end_pos, clusters.size() - 1);
    } else if (const auto fragment = fragments.back(); next_cluster_end_pos != clusters.size() && fragment.map_end_pos + 2 < map.size()) {
        fragments.emplace_back(next_end_pos, map.size() - 1, next_cluster_end_pos, clusters.size() - 1);
    }
    for (const auto &fragment: fragments) {
        if (fragment_cache.contains(fragment) && fragment_cache[fragment] == 0) { return 0; }
        assigned_clusters += fragment.clusters_end_pos - fragment.clusters_start_pos + 1;
    }
    if (assigned_clusters != clusters.size()) {
        return 0;
    }
    std::size_t permutations = 1;
    for (const auto &fragment: fragments) {
        if (!fragment_cache.contains(fragment)) {
            fragment_cache[fragment] = brute_force_fragment_permutation_count(map, clusters, fragment, fragment.map_start_pos, fragment.clusters_start_pos);
        }
        const std::size_t fragment_permutations = fragment_cache[fragment];
        if (fragment_permutations == 0) {
            return 0;
        }
        permutations *= fragment_permutations;
    }
    return permutations;
}

std::size_t brute_force_permutation_count(
    const std::string_view map,
    Clusters &clusters, const DamagedClusters &damaged_clusters,
    CollisionMap &collisions,
    const std::size_t position, const std::size_t damaged_position,
    std::vector<PermutationState> &permutation_cache,
    FragmentCache &fragment_cache
    ) {
    if (damaged_position == damaged_clusters.size() || position == clusters.size()) {
        if (is_valid_assignment(map, damaged_clusters, collisions)) {
            const auto state = PermutationState(clusters);
            auto position = std::ranges::find(permutation_cache.begin(), permutation_cache.end(), state);
            if (position != permutation_cache.end()) {
                return 0;
            }
            permutation_cache.push_back(state);
            return permutation_count(map, clusters, fragment_cache);
        }
        return 0;
    }
    // improve this
    std::size_t count = 0;
    for (int j = 0; j < static_cast<int>(clusters[position].size) - static_cast<int>(damaged_clusters[damaged_position].size) + 1; ++j) {
        const int start_pos = damaged_clusters[damaged_position].start_pos - j;
        const int end_pos = start_pos + clusters[position].size - 1;
        if (start_pos >= 0 && end_pos < map.size() &&
            (start_pos - 1 == -1 || map[start_pos - 1] != DAMAGED_SPRING) && // free space before cluster
            (end_pos + 1 == map.size() || map[end_pos + 1] != DAMAGED_SPRING) && // free space after cluster
            has_no_collisions(start_pos, end_pos, collisions)
        ) {
            clusters[position].start_pos = start_pos;
            clusters[position].end_pos = end_pos;
            for (std::size_t k = std::max(0, start_pos - 1); k <= std::min(end_pos + 1, static_cast<int>(map.size()) - 1); ++k) {
                if (collisions[k] != std::string::npos) { collisions[k]++; }
            }
            int damaged_count = 1;
            for (int k = damaged_position + 1; k < damaged_clusters.size() && damaged_clusters[k].end_pos <= end_pos; ++k ) { ++damaged_count; }
            count += brute_force_permutation_count(map, clusters, damaged_clusters, collisions, position + 1, damaged_position + damaged_count, permutation_cache, fragment_cache);
            for (std::size_t k = std::max(0, start_pos - 1); k <= std::min(end_pos + 1, static_cast<int>(map.size()) - 1); ++k) {
                if (collisions[k] != std::string::npos) { collisions[k]--; }
            }
            clusters[position].start_pos = std::string::npos;
            clusters[position].end_pos = std::string::npos;
        }
    }
    std::size_t unallocated_size = 0;
    for (int i = position; i >= 0 && clusters[i].start_pos != std::string::npos; --i) {
        unallocated_size += clusters[i].size;
    }
    if (unallocated_size <= damaged_clusters[damaged_position].unknwon_count) {
        count += brute_force_permutation_count(map, clusters, damaged_clusters, collisions, position + 1, damaged_position, permutation_cache, fragment_cache);
    }
    return count;
}

std::size_t arrangament_count(const std::string& map, Clusters& clusters) {
    const auto damaged_clusters = find_damaged(map);
    CollisionMap collisions;
    for (const auto &c: map) {
        if (c == DAMAGED_SPRING || c == UNKNOWN_SPRING) { collisions.emplace_back(0); }
        else { collisions.emplace_back(std::string::npos); }
    }
    std::vector<PermutationState> permutation_cache;
    FragmentCache fragment_cache;
    return brute_force_permutation_count(std::string_view(map), clusters, damaged_clusters, collisions, 0, 0, permutation_cache, fragment_cache);
}

std::size_t part1(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(), input.end(),
        0,
        std::plus(),
        [](const auto &line) {
            auto [map, clusters] = parse_line(line);
            return arrangament_count(map, clusters);
        }
    );
}

std::string unfold_map(const std::string& map) {
    auto unfolded_map = map;
    for (int i = 0; i < 4; ++i) {
        unfolded_map.append("?");
        unfolded_map.append(map);
    }
    return unfolded_map;
}

Clusters unfold_clusters(const Clusters& clusters) {
    Clusters unfolded_clusters;
    for (int i = 0; i < 5; ++i) {
        for (auto cluster : clusters) { unfolded_clusters.push_back(cluster); }
    }
    return unfolded_clusters;
}

std::size_t part2(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(), input.end(),
        0,
        std::plus(),
        [](const auto &line) {
            auto [map, clusters] = parse_line(line);
            auto unfolded_clusters = unfold_clusters(clusters);
            return arrangament_count(unfold_map(map), unfolded_clusters);
        }
    );
}

int main(int _, char** argv) {
    ElvenMeasure::Reporter reporter;
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    reporter.add_io_report(io_time);
    auto [result1, solution1_time] = ElvenMeasure::execute([=] { return part1(input); }, 10);
    reporter.add_report(1, result1, solution1_time);
    //auto [result2, solution2_time] = ElvenMeasure::execute([=] { return part2(input); }, 10);
    //reporter.add_report(2, result2, solution2_time);
    reporter.report();
    return 0;
}