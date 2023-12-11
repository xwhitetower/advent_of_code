#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

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

std::size_t solve(const ElvenIO::input_type &input) {
    return std::transform_reduce(
        input.begin(),
        input.end(),
        0,
        std::plus(),
        [](auto line) { return find_high(line) * 10 + find_low(line); }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}