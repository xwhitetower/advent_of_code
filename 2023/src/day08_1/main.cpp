#include <iostream>
#include <fstream>
#include <map>
#include <__numeric/transform_reduce.h>

enum instruction {
    left = 0,
    right = 1
};
typedef std::array<size_t, 2> node;
typedef std::map<size_t, node> node_maps;

constexpr size_t START_NODE = 17575;
constexpr size_t END_NODE = 0;

auto parse_instructions(const std::string &line) {
    std::vector<instruction> instructions;
    std::ranges::transform(
        line.begin(), line.end(),
        std::back_inserter(instructions),
        [](const auto &c) { return c == 'R' ? right : left; }
    );
    return instructions;
}

size_t parse_node_id(const std::string &node_str_id) {
    size_t id = 0;
    for (const char &c : node_str_id) {
        id = id * 26 + ('Z' - c);
    }
    return id;
}

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::string line;
    std::getline(file, line);
    auto instructions = parse_instructions(line);
    std::getline(file, line);

    node_maps nodes;
    while (std::getline(file, line)) {
        auto node_id = parse_node_id(line.substr(0, 3));
        auto left_node_id = parse_node_id(line.substr(7, 3));
        auto right_node_id = parse_node_id(line.substr(12, 3));
        nodes[node_id] = node{left_node_id, right_node_id};
    }

    return std::move(std::tuple(instructions, nodes));
}

size_t solve(const std::vector<instruction> &instructions, const node_maps &nodes) {
    size_t steps = 0;
    for (size_t current_node = START_NODE; current_node != END_NODE; ++steps) {
        current_node = nodes.at(current_node)[instructions[steps % instructions.size()]];
    }
    return steps;
}

int main(int _, char** argv) {
    using namespace std::chrono;

    auto [instructions, nodes] = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(instructions, nodes)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}