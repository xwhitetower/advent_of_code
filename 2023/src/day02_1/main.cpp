#include <iostream>
#include <fstream>
#include <numeric>
#include <regex>

#include "elven_measure.h"


auto get_max_color(const std::string &line, const std::regex &color_regex) {
    int amount = 0;
    auto color_begin = std::regex_iterator(line.begin(), line.end(), color_regex);
    auto color_end = std::sregex_iterator();
    for (std::sregex_iterator i = color_begin; i != color_end; ++i) {
        amount = std::max(amount, std::stoi(i->str()));
    }
    return static_cast<size_t>(amount);
}

auto parse_input(const char* filename) {
    std::fstream file(filename);
    std::vector<std::array<size_t, 3>> games;

    std::regex red_regex("(\\d+) r");
    std::regex green_regex("(\\d+) g");
    std::regex blue_regex("(\\d+) b");

    std::string line;
    while (std::getline(file, line)) {
        std::array colors {get_max_color(line, red_regex), get_max_color(line, green_regex) ,get_max_color(line, blue_regex) };
        games.emplace_back(colors);
    }

    return std::move(games);
}

auto solve(const std::vector<std::array<size_t, 3>> &games) {
    size_t sum = 0;
    for (int i = 0; i < games.size(); ++i) {
        if (games[i][0] <= 12 && games[i][1] <= 13 && games[i][2] <= 14) {
            sum += i + 1;
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