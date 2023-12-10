#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <set>
#include <map>

#include "elven_measure.h"

typedef std::vector<long> seed_list;
typedef std::set<long> origin_set;
typedef std::tuple<long, long> origin_data;
typedef std::map<long, origin_data> origin_to_data_map;
typedef std::tuple<origin_set, origin_to_data_map> transformer_data;
typedef std::vector<transformer_data> transformer_list;

auto parse_seeds(std::fstream &file) {
    seed_list seeds;

    std::string line;
    std::getline(file, line);

    std::regex number_regex("(\\d+)");
    auto seed_begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
    auto seed_end = std::sregex_iterator();
    for (std::sregex_iterator i = seed_begin; i != seed_end;  ++i) {
        seeds.emplace_back(std::stol(i->str()));
    }
    std::getline(file, line); // newline
    return std::move(seeds);
}

auto parse_transformers(std::fstream &file) {
    transformer_list transformers;

    std::string line;
    while (std::getline(file, line)) { // useless headers
        origin_set origins;
        origin_to_data_map origin_to_data;
        for(std::getline(file, line); !line.empty(); std::getline(file, line)) {
            std::stringstream stream;
            stream << line;
            long destination, origin, range;
            stream >> destination >> origin >> range;
            origins.emplace(origin);
            origin_to_data[origin] = std::tuple(destination - origin, range);
        }
        transformers.emplace_back(origins, origin_to_data);
    }

    return std::move(transformers);
}

auto parse_input(const char* filename) {
    std::fstream file(filename);
    auto seeds = parse_seeds(file);
    auto transformers = parse_transformers(file);

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

auto solve(const std::tuple<seed_list, transformer_list> &input) {
    const auto [seeds, transformers] = input;
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, transform_seed(seed, transformers));
    }
    return min_location;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}