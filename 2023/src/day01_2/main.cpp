#include <iostream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

std::size_t solve(const ElvenIO::input_type &input) {
    const std::array<std::string, 9> numbers = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

    return std::transform_reduce(
        input.begin(),
        input.end(),
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
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}