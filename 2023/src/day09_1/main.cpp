#include <numeric>
#include <regex>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::vector<long long> sequence_type;
typedef std::vector<sequence_type> input_type;

auto parse_input(const ElvenIO::input_type &input) {
    input_type sequences;

    const std::regex number_regex("(-*\\d+)");
    const auto end = std::sregex_iterator();
    for (const auto &line: input) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
        sequence_type sequence;
        for (std::sregex_iterator i = begin; i != end;  ++i) {
            sequence.emplace_back(std::stol(i->str()));
        }
        sequences.emplace_back(sequence);
    }

    return std::move(sequences);
}

std::size_t extrapolate(const sequence_type &sequence) {
    input_type differences;
    differences.push_back(sequence);
    for (
        auto current_difference = differences.back();
        current_difference.size() > 1 && !std::ranges::all_of(current_difference.begin(), current_difference.end(), [](auto reading) { return reading == 0; });
        current_difference = differences.back()
    ) {
        sequence_type next_difference;
        next_difference.reserve(current_difference.size() - 1);
        for (auto i = 1; i < current_difference.size(); ++i) {
            next_difference.emplace_back((current_difference[i] - current_difference[i-1]));
        }
        differences.emplace_back(next_difference);
    }

    return std::transform_reduce(
        differences.rbegin(), differences.rend(),
        0,
        std::plus(),
        [](auto sequence) { return sequence.back() ; }
    );
}

auto solve(const ElvenIO::input_type &input) {
    const auto sequences = parse_input(input);
    return std::transform_reduce(
        sequences.begin(), sequences.end(),
        0,
        std::plus(),
        [](auto sequence) { return extrapolate(sequence); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}