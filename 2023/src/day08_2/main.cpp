#include <iostream>
#include <fstream>
#include <map>
#include <numeric>

#include "elven_measure.h"

enum instruction {
    left = 0,
    right = 1
};
typedef std::string node_id;
typedef std::array<node_id, 2> node;
typedef std::map<node_id, node> node_maps;
constexpr char START_MARKER = 'A';
constexpr char END_MARKER = 'Z';

auto parse_instructions(const std::string &line) {
    std::vector<instruction> instructions;
    std::ranges::transform(
        line.begin(), line.end(),
        std::back_inserter(instructions),
        [](const auto &c) { return c == 'R' ? right : left; }
    );
    return instructions;
}

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::string line;
    std::getline(file, line);
    auto instructions = parse_instructions(line);
    std::getline(file, line);

    node_maps nodes;
    std::vector<node_id> start_nodes;
    while (std::getline(file, line)) {
        const node_id current_node = line.substr(0, 3);
        if (current_node.back() == START_MARKER) { start_nodes.push_back(current_node); }
        nodes[current_node] = node{line.substr(7, 3), line.substr(12, 3)};
    }

    return std::move(std::tuple(instructions, start_nodes, nodes));
}

auto solve_single(const node_id &start_node, const size_t start_steps, const std::vector<instruction> &instructions,  const node_maps &nodes) {
    std::string current_node = nodes.at(start_node)[instructions[start_steps % instructions.size()]];
    size_t steps = start_steps + 1;
    for (;current_node.back() != END_MARKER; ++steps) {
        current_node = nodes.at(current_node)[instructions[steps % instructions.size()]];
    }
    return steps;
}

size_t solve(const std::tuple<std::vector<instruction>, std::vector<node_id>, node_maps> &input) {
    const auto [instructions, start_nodes, nodes] = input;
    std::vector<size_t> cycles;
    std::ranges::transform(
        start_nodes.begin(), start_nodes.end(),
        std::back_inserter(cycles),
        [&instructions, &nodes](auto node) { return solve_single(node, 0, instructions, nodes); }
    );

    return std::accumulate(
        cycles.begin(), cycles.end(),
        static_cast<std::size_t>(1),
        [](const auto & a, const auto & b) {
        return std::lcm(a, b);
    });
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}