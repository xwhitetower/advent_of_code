#include <iostream>
#include <fstream>
#include <map>

#include "elven_measure.h"

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

size_t solve(const std::tuple<std::vector<instruction>, node_maps> &input) {
    const auto [instructions, nodes] = input;
    size_t steps = 0;
    for (size_t current_node = START_NODE; current_node != END_NODE; ++steps) {
        current_node = nodes.at(current_node)[instructions[steps % instructions.size()]];
    }
    return steps;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}