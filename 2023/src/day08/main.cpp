#include <map>
#include <numeric>

#include "elven_io.h"
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

auto parse_input(const ElvenIO::input_type &input) {
    auto instructions = parse_instructions(input[0]);

    node_maps nodes;
    std::vector<node_id> start_nodes;
    for (int i = 2; i < input.size(); ++i) {
        const auto line = input[i];
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

auto part1(const ElvenIO::input_type &input) {
    // Join with part2 eventually.
    const auto [instructions, _, nodes] = parse_input(input);
    size_t steps = 0;
    for (std::string current_node = "AAA"; current_node != "ZZZ"; ++steps) {
        current_node = nodes.at(current_node)[instructions[steps % instructions.size()]];
    }
    return steps;
}

auto part2(const ElvenIO::input_type &input) {
    const auto [instructions, start_nodes, nodes] = parse_input(input);
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