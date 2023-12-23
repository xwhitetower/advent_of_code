#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

std::size_t part1(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(),
        input.end(),
        0,
        std::plus(),
        [](auto line) {
            auto high = std::find_if(line.begin(), line.end(), isdigit);
            auto low = std::find_if(line.rbegin(), line.rend(), isdigit);
            return (*high - '0') * 10 + (*low - '0');
        }
    );
}

const std::array<std::string, 9> numbers = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

std::optional<std::size_t> is_number(const std::string &line, const int i) {
    const char c = line[i];
    if (isdigit(c)) {
        return std::optional(c - '0');
    }
    if (c == 'o' || c == 't' || c == 'f' || c == 's' || c == 'e' || c == 'n') {
        for (int j = 0 ; j < numbers.size(); j++) {
            if (strncmp(&line[i], numbers[j].c_str(), numbers[j].length()) == 0) {
                return std::optional(j + 1);
            }
        }
    }
    return std::nullopt;
}

std::size_t find_high(const std::string &line) {
    for (int i = 0 ; i < line.length(); ++i) {
        if (const auto high = is_number(line, i); high.has_value()) { return high.value(); }
    }
    throw std::runtime_error("number not found");
}

std::size_t find_low(const std::string &line) {
    for (int i = line.length() - 1 ; i >= 0; --i) {
        if (const auto low = is_number(line, i); low.has_value()) { return low.value(); }
    }
    throw std::runtime_error("number not found");
}

std::size_t part2(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(),
        input.end(),
        0,
        std::plus(),
        [](auto line) { return find_high(line) * 10 + find_low(line); }
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