#include <iostream>
#include <fstream>
#include <numeric>

#include "elven_measure.h"

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::vector<std::string> lines;

    std::copy(
        std::istream_iterator<std::string>(file),
        std::istream_iterator<std::string>(),
        std::back_inserter(lines)
    );

    return std::move(lines);
}

std::size_t solve(const std::vector<std::string> &lines) {
    return std::transform_reduce(
        lines.begin(),
        lines.end(),
        0,
        std::plus(),
        [](auto line) {
            auto high = std::find_if(line.begin(), line.end(), isdigit);
            auto low = std::find_if(line.rbegin(), line.rend(), isdigit);
            return (*high - '0') * 10 + (*low - '0');
        }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}