#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

std::vector<std::string> parse_input(const ElvenIO::input_type &input) {
    std::vector<std::string> sequences;
    std::stringstream stream;
    stream << input[0];
    std::string sequence;
    while(std::getline(stream, sequence, ',')) {
        sequences.emplace_back(sequence);
    }
    return std::move(sequences);
}

std::size_t elf_hash(const std::string &sequence) {
    std::size_t hash = 0;
    for (const auto &c: sequence) {
        hash = (hash + c) * 17 % 256;
    }
    return hash;
}

std::size_t solve(const ElvenIO::input_type &input) {
    const auto sequences = parse_input(input);
    return std::transform_reduce(
        sequences.begin(), sequences.end(),
        0,
        std::plus(),
        [](const auto &sequence) { return elf_hash(sequence); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}