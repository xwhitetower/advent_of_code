#include <sstream>

#include "elven_io.h"
#include "elven_measure.h"


typedef std::vector<std::string> Sequences;
typedef std::array<std::vector<std::string>, 256> Boxes;

constexpr char REMOVE_TOKEN = '-';

Sequences parse_input(const ElvenIO::input_type &input) {
    Sequences sequences;
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

void execute_sequences(const Sequences &sequences, Boxes &boxes) {
    for (const auto &sequence: sequences) {
        if (sequence.back() == REMOVE_TOKEN) {
            std::string label = sequence.substr(0, sequence.size() -1);
            auto &box = boxes[elf_hash(label)];
            std::string comp_label = label + "=";
            std::erase_if(box, [&comp_label](const auto &elem) { return elem.starts_with(comp_label); });
        } else {
            std::string label = sequence.substr(0, sequence.size() -2);
            auto &box = boxes[elf_hash(label)];
            std::string comp_label = label + "=";
            auto found = std::ranges::find_if(box.begin(), box.end(), [&comp_label](const auto &elem) { return elem.starts_with(comp_label); });
            if (found == box.end()) {
                box.push_back(sequence);
            } else {
                *found = sequence;
            }
        }
    }
}

// 259536 too high
std::size_t focusing_power(const Boxes &boxes) {
    std::size_t power = 0;
    for (int i = 0; i < boxes.size(); ++i) {
        const auto &lenses = boxes[i];
        for (int j = 0; j < lenses.size(); ++j) {
            power += (i + 1) * (j + 1) * (lenses[j].back() - '0');
        }
    }
    return power;
}

std::size_t solve(const ElvenIO::input_type &input) {
    const auto sequences = parse_input(input);
    Boxes boxes;
    execute_sequences(sequences, boxes);
    return focusing_power(boxes);
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 1);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}