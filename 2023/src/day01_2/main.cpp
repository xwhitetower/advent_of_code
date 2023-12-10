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
    const std::array<std::string, 9> numbers = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

    return std::transform_reduce(
        lines.begin(),
        lines.end(),
        0,
        std::plus(),
        [=](auto line) {
            // THIS IS AWFUL FIX
            size_t high = -1;
            size_t low = 0;
            for ( int i = 0 ; i < line.length(); i++) {
                int digit = -1;
                if (isdigit(line[i])) {
                    digit = line[i] - '0';
                } else {
                    for(int j = 0 ; j < numbers.size(); j++) {
                        if (strncmp(&line[i], numbers[j].c_str(), numbers[j].length()) == 0) {
                            digit = j + 1;
                            break;
                        }
                    }
                }
                if (digit != -1) {
                    high = high == -1 ? digit : high;
                    low = digit;
                }
            }
            return high * 10 + low;
        }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}