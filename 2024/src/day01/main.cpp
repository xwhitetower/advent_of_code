#include <map>
#include <numeric>
#include <sstream>

#include "elven_io.h"
#include "elven_measure.h"


auto parse(const ElvenIO::input_type &input) {
    std::vector<int> left_list;
    std::vector<int> right_list;
    for (const auto &line : input) {
        std::stringstream stream;
        stream << line;
        int left, right;
        stream >> left >> right;
        left_list.push_back(left);
        right_list.push_back(right);
    }
    return std::make_pair(left_list, right_list);
}

std::size_t part1(const ElvenIO::input_type &input) {
    auto [left_list, right_list] = parse(input);
    std::ranges::sort(left_list);
    std::ranges::sort(right_list);
    std::size_t result = 0;
    for (auto index = 0; index < left_list.size(); ++index) {
        result += std::abs(right_list[index] - left_list[index]);
    }
    return result;
}

std::size_t part2(const ElvenIO::input_type &input) {
    auto [left_list, right_list] = parse(input);

    std::map<int, std::size_t> right_map_tally;
    for (auto& right_value : right_list) { ++right_map_tally[right_value]; }
    return std::transform_reduce(
        left_list.begin(), left_list.end(),
        0,
        std::plus(),
        [&right_map_tally](auto left_value) { return left_value * right_map_tally[left_value];}
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