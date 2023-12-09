#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <set>

typedef std::tuple<long, long> seed_range;
typedef std::vector<seed_range> seed_list;
typedef std::tuple<long, long, long> transformer_data;
typedef std::vector<transformer_data> stage_data;
typedef std::vector<stage_data> stage_list;

auto parse_seeds(std::fstream &file) {
    seed_list seeds;

    std::string line;
    std::getline(file, line);

    std::regex number_regex("(\\d+)");
    auto seed_begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
    auto seed_end = std::sregex_iterator();
    for (std::sregex_iterator i = seed_begin; i != seed_end;  ++i) {
        auto seed = std::stol(i->str());
        auto range = std::stol((++i)->str());
        seeds.emplace_back(seed, range);
    }
    std::getline(file, line); // newline
    return std::move(seeds);
}

auto parse_transformers(std::fstream &file) {
    stage_list stages;

    std::string line;
    while (std::getline(file, line)) { // useless headers
        stage_data stage;
        for(std::getline(file, line); !line.empty(); std::getline(file, line)) {
            std::stringstream stream;
            stream << line;
            long destination, origin, range;
            stream >> destination >> origin >> range;
            stage.emplace_back(origin, destination, range);
        }
        std::sort(stage.begin(), stage.end());
        stages.emplace_back(stage);
    }

    return std::move(stages);
}

auto parse_input(const char* filename) {
    std::fstream file(filename);
    auto seeds = parse_seeds(file);
    auto transformers = parse_transformers(file);
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

auto solve(const seed_list &seeds, const stage_list &stages) {
    auto min_location = std::numeric_limits<long>::max();
    for (const auto seed: seeds) {
        min_location = std::min(min_location, solve(seed, stages, 0, 0));
    }
    return min_location;
}

int main(int _, char** argv) {
    using namespace std::chrono;

    const auto [seeds, stages] = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(seeds, stages)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}
