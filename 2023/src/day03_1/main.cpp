#include <iostream>
#include <fstream>
#include <numeric>

#include "elven_measure.h"

constexpr auto EMPTY = '.';

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

size_t parse_number(const std::vector<std::string> &schematic, const int x, const int y) {
    auto number = 0;
    for (int i = x, mult = 1; i >= 0 && isdigit(schematic[y][i]); --i, mult *= 10) {
        number = number + (schematic[y][i] - '0') * mult;
    }
    for (int i = x + 1; i < schematic[y].size() && isdigit(schematic[y][i]); ++i) {
        number = number * 10 + (schematic[y][i] - '0');
    }
    return number;
}

size_t solve(const std::vector<std::string> &schematic) {
    size_t sum = 0;
    for (int y = 0; y < schematic.size(); ++y) {
        for (int x = 0; x < schematic[y].size(); ++x) {
            if (schematic[y][x] != EMPTY && !isdigit(schematic[y][x])) {
                for (int y_i = std::max(y-1, 0); y_i <= y+1 && y_i < schematic.size(); y_i++) {
                    for (int x_i = std::max(x-1, 0); x_i <= x+1 && x_i < schematic[y_i].size(); x_i++) {
                        if (isdigit(schematic[y_i][x_i]) &&
                            (
                                y == y_i // no extra checks on horizontal
                                || x_i < x // no extra checks on the left corners
                                || !isdigit(schematic[y_i][x_i-1]) // not already processed
                            )
                        ) {
                            sum += parse_number(schematic,x_i, y_i);
                        }
                    }
                }
            }
        }
    }
    return sum;
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return parse_input(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}