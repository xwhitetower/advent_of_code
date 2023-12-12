#include <sstream>
#include <numeric>

#include "elven_io.h"
#include "elven_measure.h"

typedef std::tuple<size_t, size_t> race;

auto parse_input(const ElvenIO::input_type &input) {
    std::vector<race> races;

    std::stringstream timestream;
    timestream << input[0];
    std::stringstream distancestream;
    distancestream << input[1];
    std::string header1, header2;
    timestream >> header1;
    distancestream >> header2;

    size_t time, distance;
    while (timestream >> time && distancestream >> distance) { races.emplace_back(time, distance); }
    return races;
}

size_t search_min_win_position(const size_t &distance, const size_t &time) {
    size_t min_win = distance;
    for (size_t high = time, low = 0, mid = (high + low) / 2; low <= high; mid = (high + low) / 2.0) {
        if (const auto run_distance = (time - mid) * mid; run_distance > distance) {
            min_win = mid;
            high = high == mid ? mid - 1 : mid;
        } else {
            low = low == mid ? mid + 1 : mid;
        }
    }
    return min_win;
}

size_t search_max_win_position(const size_t &distance, const size_t &time) {
    size_t max_win = 0;
    for (size_t high = time, low = 0, mid = (high + low) / 2; low <= high; mid = (high + low) / 2.0) {
        if (const auto run_distance = (time - mid) * mid; run_distance > distance) {
            max_win = mid;
            low = low == mid ? mid + 1 : mid;
        } else {
            high = high == mid ? mid - 1 : mid;
        }
    }
    return max_win;
}

auto solve(const ElvenIO::input_type &input) {
    const auto races = parse_input(input);
    return std::transform_reduce(
        races.begin(),
        races.end(),
        1,
        std::multiplies(),
        [](auto race) {
            auto [time, distance] = race;
            return search_max_win_position(distance, time) - search_min_win_position(distance, time) + 1;
        }
    );
}

int main(int _, char** argv) {
    const auto [input, io_time] = ElvenMeasure::execute([=]{ return ElvenIO::read(argv[1]); });
    auto [result, solution_time] = ElvenMeasure::execute([=] { return solve(input); }, 100);
    ElvenMeasure::report(result, io_time, solution_time);
    return 0;
}