#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <set>
#include <map>

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

auto solve(const seed_list &seeds, const transformer_list &transformers) {
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, transform_seed(seed, transformers));
    }
    return min_location;
}

int main(int _, char** argv) {
    using namespace std::chrono;

    const auto [seeds, transformers] = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(seeds, transformers)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}
