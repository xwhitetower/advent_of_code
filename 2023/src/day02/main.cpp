#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

auto parse_input(const ElvenIO::input_type &input) {
    std::vector<std::vector<size_t>> games;

    for (const auto &line: input) {
        std::stringstream stream;
        std::string color;
        std::size_t number;
        stream << line;
        stream >> color >> color;
        std::vector<std::size_t> colors(3, 0);
        while(stream >> number >> color) {
            switch (color[0]) {
                case 'r':
                    colors[0] = std::max(colors[0], number);
                break;
                case 'g':
                    colors[1] = std::max(colors[1], number);
                break;
                case 'b':
                    colors[2] = std::max(colors[2], number);
                break;
            }
        }
        games.emplace_back(colors);
    }

    return std::move(games);
}

auto part1(const ElvenIO::input_type &input) {
    size_t sum = 0;
    const auto games = parse_input(input);
    for (int i = 0; i < games.size(); ++i) {
        if (games[i][0] <= 12 && games[i][1] <= 13 && games[i][2] <= 14) {
            sum += i + 1;
        }
    }
    return sum;
}

auto part2(const ElvenIO::input_type &input) {
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