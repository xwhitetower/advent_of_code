#include <iostream>
#include <numeric>
#include <regex>

#include "elven_io.h"
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

auto parse_input(const ElvenIO::input_type &input) {
    std::vector<std::array<size_t, 3>> games;

    const std::regex red_regex("(\\d+) r");
    const std::regex green_regex("(\\d+) g");
    const std::regex blue_regex("(\\d+) b");

    for(const auto &line: input) {
        std::array colors {get_max_color(line, red_regex), get_max_color(line, green_regex) ,get_max_color(line, blue_regex) };
        games.emplace_back(colors);
    }

    return std::move(games);
}

auto solve(const ElvenIO::input_type &input) {
    const auto games = parse_input(input);
    return std::transform_reduce(
        games.begin(),
        games.end(),
        0,
        std::plus(),
        [] (auto game) { return std::reduce(game.begin(), game.end(), 1, std::multiplies<size_t>()); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}