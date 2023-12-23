#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::vector<long long> sequence_type;
typedef std::vector<sequence_type> input_type;
typedef long long solution_type;

auto parse_input(const ElvenIO::input_type &input) {
    input_type sequences;

    for (const auto &line: input) {
        std::stringstream stream;
        stream << line;
        long long value;
        sequence_type sequence;
        while (stream >> value) { sequence.emplace_back(value); }
        sequences.emplace_back(sequence);
    }

    return std::move(sequences);
}

input_type compute_differences(const sequence_type &sequence) {
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
    return differences;
}

solution_type extrapolate_forward(const sequence_type &sequence) {
    const auto differences = compute_differences(sequence);

    return std::transform_reduce(
        differences.rbegin(), differences.rend(),
        0,
        std::plus(),
        [](auto sequence) { return sequence.back() ; }
    );
}

auto part1(const ElvenIO::input_type &input) {
    const auto sequences = parse_input(input);
    return std::transform_reduce(
        sequences.begin(), sequences.end(),
        0,
        std::plus(),
        [](auto sequence) { return extrapolate_forward(sequence); }
    );
}

solution_type extrapolate_past(const sequence_type &sequence) {
    const auto differences = compute_differences(sequence);

    solution_type extrapolated = 0;
    for (int i = differences.size() - 2; i >= 0; --i) {
        extrapolated = differences[i].front() - extrapolated;
    }
    return extrapolated;
}

auto part2(const ElvenIO::input_type &input) {
    const auto sequences = parse_input(input);
    return std::transform_reduce(
        sequences.begin(), sequences.end(),
        0,
        std::plus(),
        [](auto sequence) { return extrapolate_past(sequence); }
    );
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