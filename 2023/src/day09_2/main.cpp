#include <iostream>
#include <fstream>
#include <numeric>
#include <regex>

typedef std::vector<long long> sequence_type;
typedef std::vector<sequence_type> input_type;
typedef long long solution_type;

auto parse_input(const char* filename) {
    std::fstream file(filename);
    input_type sequences;

    std::string line;
    const std::regex number_regex("(-*\\d+)");
    const auto end = std::sregex_iterator();
    while (std::getline(file, line)) {
        auto begin = std::sregex_iterator(line.begin(), line.end(), number_regex);
        sequence_type sequence;
        for (std::sregex_iterator i = begin; i != end;  ++i) {
            sequence.emplace_back(std::stol(i->str()));
        }
        sequences.emplace_back(sequence);
    }

    return std::move(sequences);
}

solution_type extrapolate(const sequence_type &sequence) {
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

    solution_type extrapolated = 0;
    for (int i = differences.size() - 2; i >= 0; --i) {
        extrapolated = differences[i].front() - extrapolated;
    }
    return extrapolated;
}

solution_type solve(const input_type &sequences) {
    return std::transform_reduce(
        sequences.begin(), sequences.end(),
        0,
        std::plus(),
        [](auto sequence) { return extrapolate(sequence); }
    );
}

int main(int _, char** argv) {
    using namespace std::chrono;

    const auto input = parse_input(argv[1]);
    const auto start {high_resolution_clock::now()};
    const auto solution {solve(input)};
    const auto end {high_resolution_clock::now()};
    std::cout << solution << std::endl;
    const duration<double> elapsed_seconds{end - start};
    std::cout << elapsed_seconds.count() * 1000 << "ms" << std::endl;
    std::cout << elapsed_seconds.count() * 1000000 << "µs" << std::endl;
    return 0;
}