#include <iostream>
#include <fstream>
#include <map>
#include <numeric>

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

size_t solve(
    const std::vector<instruction> &instructions,
    const std::vector<node_id> &start_nodes,
    const node_maps &nodes
    ) {
    std::vector<size_t> cycles;
    std::ranges::transform(
        start_nodes.begin(), start_nodes.end(),
        std::back_inserter(cycles),
        [=](auto node) { return solve_single(node, 0, instructions, nodes); }
    );

    return std::accumulate(
        cycles.begin(), cycles.end(),
        static_cast<std::size_t>(1),
        [](const auto & a, const auto & b) {
        return std::lcm(a, b);
    });
}

int main(int _, char** argv) {
    using namespace std::chrono;

    auto [instructions, start_nodes, nodes] = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(instructions, start_nodes, nodes)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}