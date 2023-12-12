#include <iostream>
#include <sstream>
#include <set>
#include <map>

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

auto solve(const ElvenIO::input_type &input) {
    const auto [seeds, transformers] = parse_input(input);
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, transform_seed(seed, transformers));
    }
    return min_location;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}