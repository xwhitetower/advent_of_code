#include <iostream>
#include <sstream>
#include <regex>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::tuple<long, long> seed_range;
typedef std::vector<seed_range> seed_list;
typedef std::tuple<long, long, long> transformer_data;
typedef std::vector<transformer_data> stage_data;
typedef std::vector<stage_data> stage_list;

auto parse_seeds(const ElvenIO::input_type &input) {
    seed_list seeds;

    std::regex number_regex("(\\d+)");
    auto seed_begin = std::sregex_iterator(input[0].begin(), input[0].end(), number_regex);
    auto seed_end = std::sregex_iterator();
    for (std::sregex_iterator i = seed_begin; i != seed_end;  ++i) {
        auto seed = std::stol(i->str());
        auto range = std::stol((++i)->str());
        seeds.emplace_back(seed, range);
    }
    return std::move(seeds);
}

auto parse_transformers(const ElvenIO::input_type &input) {
    stage_list stages;

    for (int i = 3; i < input.size(); i += 2) {
        stage_data stage;
        for (; !input[i].empty(); ++i) {
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

auto parse_input(const ElvenIO::input_type &input) {
    auto seeds = parse_seeds(input);
    auto transformers = parse_transformers(input);
    return std::move(std::tuple(seeds, transformers));
}

auto solve(const seed_range &seed, const stage_list &stages, const size_t stage_index, const size_t transformer_index) {
    auto [origin, range] = seed;
    if (range <= 0) {
        return std::numeric_limits<long>::max();
    }
    if (stage_index == stages.size()) {
        return origin;
    }
    if (transformer_index == stages[stage_index].size()) {
        return solve(seed, stages, stage_index + 1, 0);
    }
    auto [transformer_origin, transformer_destination, transformer_range] = stages[stage_index][transformer_index];
    if (origin + range < transformer_origin) { // data is sorted, so we can move to the next stage
        return solve(seed, stages, stage_index + 1, 0);
    }
    if (transformer_origin + transformer_range < origin) { // not arrived to any relevant data transformation
        return solve(seed, stages, stage_index, transformer_index + 1);
    }
    auto minimum_location = std::numeric_limits<long>::max();
    if (origin < transformer_origin) {
        minimum_location = std::min(minimum_location, solve(seed_range(origin, transformer_origin - origin - 1), stages, stage_index + 1, 0));
        origin = transformer_origin;
        range -= transformer_origin - origin - 1;
    }
    auto offset = transformer_destination - transformer_origin;
    minimum_location  = std::min(
        minimum_location,
        solve(seed_range(origin + offset, std::min(transformer_origin + transformer_range - origin, range)), stages, stage_index + 1, 0)
    );
    return std::min(
        minimum_location,
        solve(seed_range(transformer_origin + transformer_range, range - (transformer_origin + transformer_range - origin)), stages, stage_index, transformer_index + 1)
    );
}

auto solve(const ElvenIO::input_type &input) {
    const auto [seeds, stages] = parse_input(input);
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, solve(seed, stages, 0, 0));
    }
    return min_location;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}