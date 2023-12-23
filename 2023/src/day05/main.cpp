// part 1 & 2 when they were different files did not re-use anything. Homogenize code later.
#include <set>
#include <map>
#include <iostream>
#include <sstream>

#include "elven_io.h"
#include "elven_measure.h"


typedef std::vector<long> seed_list;
typedef std::set<long> origin_set;
typedef std::tuple<long, long> origin_data;
typedef std::map<long, origin_data> origin_to_data_map;
typedef std::tuple<origin_set, origin_to_data_map> transformer_data;
typedef std::vector<transformer_data> transformer_list;

auto parse_seeds(const ElvenIO::input_type &input) {
    seed_list seeds;

    std::stringstream stream;
    stream << input[0];
    std::string header;
    stream >> header;
    long seed;
    while (stream >> seed) { seeds.emplace_back(seed);  }
    return std::move(seeds);
}

auto parse_transformers(const ElvenIO::input_type &input) {
    transformer_list transformers;

    for (int i = 3; i < input.size(); i += 2) {
        origin_set origins;
        origin_to_data_map origin_to_data;
        for (; i < input.size() && !input[i].empty(); ++i) {
            std::stringstream stream;
            stream << input[i];
            long destination, origin, range;
            stream >> destination >> origin >> range;
            origins.emplace(origin);
            origin_to_data[origin] = std::tuple(destination - origin, range);
        }
        transformers.emplace_back(origins, origin_to_data);
    }

    return std::move(transformers);
}

auto parse_input(const ElvenIO::input_type &input) {
    auto seeds = parse_seeds(input);
    auto transformers = parse_transformers(input);

    return std::move(std::tuple(seeds, transformers));
}

auto transform_seed(long seed, const transformer_list &transformers) {
    for (auto [origins, origin_to_data]: transformers) {
        auto nearest = std::find_if(
            origins.rbegin(), origins.rend(),
            [=](auto elem) { return elem <= seed; }
        );
        if (nearest != origins.rend()) {
            if (auto [offset, range] = origin_to_data[*nearest]; seed < *nearest + range) {
                seed += offset;
            }
        }
    }
    return seed;
}

auto part1(const ElvenIO::input_type &input) {
    const auto [seeds, transformers] = parse_input(input);
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, transform_seed(seed, transformers));
    }
    return min_location;
}

typedef std::tuple<long, long> seed_range;
typedef std::vector<seed_range> seed_list2;
typedef std::tuple<long, long, long> transformer_data2;
typedef std::vector<transformer_data2> stage_data;
typedef std::vector<stage_data> stage_list;

auto parse_seeds2(const ElvenIO::input_type &input) {
    seed_list2 seeds;

    std::stringstream stream;
    stream << input[0];
    std::string header;
    stream >> header;
    long seed_start, count;
    while (stream >> seed_start >> count) { seeds.emplace_back(seed_start, count);  }
    return std::move(seeds);
}

auto parse_transformers2(const ElvenIO::input_type &input) {
    stage_list stages;

    for (int i = 3; i < input.size(); i += 2) {
        stage_data stage;
        for (; i < input.size() && !input[i].empty(); ++i) {
            std::stringstream stream;
            stream << input[i];
            long destination, origin, range;
            stream >> destination >> origin >> range;
            stage.emplace_back(origin, destination, range);
        }
        std::sort(stage.begin(), stage.end());
        stages.emplace_back(stage);
    }

    return std::move(stages);
}

auto parse_input2(const ElvenIO::input_type &input) {
    auto seeds = parse_seeds2(input);
    auto transformers = parse_transformers2(input);
    return std::move(std::tuple(seeds, transformers));
}

auto solve2(const seed_range &seed, const stage_list &stages, const size_t stage_index, const size_t transformer_index) {
    auto [origin, range] = seed;
    if (range <= 0) {
        return std::numeric_limits<long>::max();
    }
    if (stage_index == stages.size()) {
        return origin;
    }
    if (transformer_index == stages[stage_index].size()) {
        return solve2(seed, stages, stage_index + 1, 0);
    }
    auto [transformer_origin, transformer_destination, transformer_range] = stages[stage_index][transformer_index];
    if (origin + range < transformer_origin) { // data is sorted, so we can move to the next stage
        return solve2(seed, stages, stage_index + 1, 0);
    }
    if (transformer_origin + transformer_range < origin) { // not arrived to any relevant data transformation
        return solve2(seed, stages, stage_index, transformer_index + 1);
    }
    auto minimum_location = std::numeric_limits<long>::max();
    if (origin < transformer_origin) {
        minimum_location = std::min(minimum_location, solve2(seed_range(origin, transformer_origin - origin - 1), stages, stage_index + 1, 0));
        origin = transformer_origin;
        range -= transformer_origin - origin - 1;
    }
    const auto offset = transformer_destination - transformer_origin;
    minimum_location  = std::min(
        minimum_location,
        solve2(seed_range(origin + offset, std::min(transformer_origin + transformer_range - origin, range)), stages, stage_index + 1, 0)
    );
    return std::min(
        minimum_location,
        solve2(seed_range(transformer_origin + transformer_range, range - (transformer_origin + transformer_range - origin)), stages, stage_index, transformer_index + 1)
    );
}

auto part2(const ElvenIO::input_type &input) {
    const auto [seeds, stages] = parse_input2(input);
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, solve2(seed, stages, 0, 0));
    }
    return min_location;
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